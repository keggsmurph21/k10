use std::sync::Arc;
use tokio::io::AsyncReadExt;
use tokio::sync::RwLock;
use tokio_tungstenite::tungstenite as tt;

use crate::server::api;
use crate::server::api::Message;
use crate::server::error;
use crate::server::error::ClientError;

use super::state;

pub async fn handle_stdin(
    _state: Arc<RwLock<state::State>>,
    tx: futures_channel::mpsc::UnboundedSender<tt::Message>,
) {
    let mut stdin = tokio::io::stdin();
    loop {
        let mut buf = vec![0; 1024];
        let n = match stdin.read(&mut buf).await {
            Err(_) | Ok(0) => break,
            Ok(n) => n,
        };
        buf.truncate(n);

        match std::str::from_utf8(&buf)
            .map(String::from)
            .map_err(ClientError::Utf8Error)
            .and_then(api::Request::from_msg)
            .and_then(|msg| msg.to_msg())
            .and_then(|msg| {
                tx.unbounded_send(msg)
                    .map_err(ClientError::ChannelWriteError)
            }) {
            Ok(_) => {}
            Err(e) => eprintln!("{:?}", e),
        }
    }
}

pub async fn handle_msg(
    state: Arc<RwLock<state::State>>,
    msg: std::result::Result<tt::Message, tt::Error>,
) {
    use api::Response::*;
    use api::*;

    let msg = match msg
        .map_err(error::ClientError::WebsocketError)
        .and_then(api::Response::from_msg)
    {
        Err(e) => {
            // The server is probably down.
            panic!("Error reading from websocket: {:?}", e);
        }
        Ok(msg) => msg,
    };

    // TODO: Remove this in favor of some real logging.  We
    //       can already see that this approach doesn't work
    //       for large messages like CurrentView ...
    //if matches!(msg, InGame(InGameResponse::CurrentView(_))) {
    //eprintln!("Received updated view");
    //} else {
    eprintln!("Received {:?}", msg);
    //}

    let mut state = state.write().await;

    match (state.authenticated, msg) {
        (_, ClientError(_)) => {
            // TODO: We *might* be able to continue in some cases
            //       here, depending on how badly we screwed up
            //       (for example, someone may have beaten us to
            //       accepting a trade).
            panic!();
        }
        (_, ServerError(e)) => {
            // We almost definitely cannot
            panic!("{:?}", e);
        }
        (false, Lobby(LobbyResponse::Authenticated(users))) => state.authenticate(users),
        (true, Lobby(LobbyResponse::Authenticated(_))) => {
            // it's a 🐞!
            panic!("Already authenticated");
        }
        (false, _) => {
            // it's a 🐞!
            panic!("Unexpected message from server before validation");
        }
        (_, Lobby(LobbyResponse::UserConnected(user))) => state.user_connected(user),
        (_, Lobby(LobbyResponse::UserDisconnected(user))) => state.user_disconnected(user),
        (_, Lobby(LobbyResponse::CreatedGame(pending_game)))
        | (_, Lobby(LobbyResponse::PlayersChanged(pending_game))) => {
            state.update_pending_game(pending_game)
        }
        (_, Lobby(LobbyResponse::GameStarted(game_id))) => state.remove_pending_game(&game_id),
        (_, InGame(InGameResponse::CurrentView(view))) => state.update_view(view),
        (_, InGame(InGameResponse::MadeMove(res))) => state.made_move(res),
    };

    // TODO: An actual UI :^)
    //eprintln!("* users: {:?}", state.users);
    //eprintln!("* games: {:?}", state.pending_games);
    //eprintln!("* curr:  {:?}", state.current_game);
}
