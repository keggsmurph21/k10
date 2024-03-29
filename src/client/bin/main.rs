use futures::SinkExt;
use futures_util::{future, pin_mut, StreamExt};
use std::sync::Arc;
use tokio::sync::RwLock;

extern crate k10;

use k10::client::handler;
use k10::client::me;
use k10::client::state;
use k10::server::api;
use k10::server::api::Message;
use k10::server::error::ClientError;
use k10::server::result::Result;

pub async fn connect() -> Result<()> {
    // First, we need register a new user.  At some point, it
    // would probably make sense to take in the username and
    // password as command-line arguments (or read them from
    // a credentials file or w/e).  For testing, though, this
    // is the most convenient :^)
    let me = me::Me::random().await?;

    // Connect to the websocket server.
    //
    // TODO: Make the host/port configurable!
    let url = format!("ws://localhost:3088/ws/{}", &me.id);
    let (ws_stream, _) = tokio_tungstenite::connect_async(url)
        .await
        .map_err(ClientError::WebsocketError)?;
    let (mut writer, reader) = ws_stream.split();

    // Once the websocket & channels are set up, immediately
    // try to authenticate.
    let req = api::AuthenticateRequest { token: me.token };
    let req = api::UnauthenticatedRequest::Authenticate(req);
    let req = api::Request::Unauthenticated(req);
    writer.send(req.to_msg()?).await?;

    // We'll maintain all of the state of our client application
    // in this single thread-safe object.
    let state = Arc::new(RwLock::new(state::State::new(me)));

    // Handle input on stdin in a background thread.
    let (stdin_tx, stdin_rx) = futures_channel::mpsc::unbounded();
    tokio::spawn(handler::handle_stdin(state.clone(), stdin_tx));

    // Forward messages directly from our stdin reader => websocket.
    // These have already been parsed into valid api::Request's.
    let stdin_to_ws = stdin_rx.map(Ok).forward(writer);

    // Forward messages directly from the websocket => stdout
    // (after parsing).  In the future, we may want to intercept
    // these messages in a different dedicated thread.
    //
    // For now, though, this handler just gets a reference to
    // the "global" application state (which it's free to mutate).
    let ws_to_stdout = reader.for_each(|msg| handler::handle_msg(state.clone(), msg));

    // tokio magic
    pin_mut!(stdin_to_ws, ws_to_stdout);
    future::select(stdin_to_ws, ws_to_stdout).await;

    Ok(())
}

#[tokio::main]
async fn main() {
    connect().await.unwrap();
}
