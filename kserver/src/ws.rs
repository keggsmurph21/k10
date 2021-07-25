use futures::{FutureExt, StreamExt};
use sqlx::SqlitePool;
use tokio::sync::mpsc;
use tokio_stream::wrappers::UnboundedReceiverStream;
use warp::ws::WebSocket;

use super::client::{Client, ClientState, Clients};
use super::handler::handle_message;
use super::kapi;
use super::kapi::Message;
use super::model::user::UserId;

pub async fn on_client_connection(ws: WebSocket, id: UserId, pool: SqlitePool, clients: Clients) {
    let (client_send_ws, mut client_recv_ws) = ws.split();
    let (client_send_chan, client_recv_chan) = mpsc::unbounded_channel();

    tokio::task::spawn(
        UnboundedReceiverStream::new(client_recv_chan)
            .forward(client_send_ws)
            .map(|result| {
                if let Err(e) = result {
                    eprintln!("Error sending ws msg: {:?}", e);
                }
            }),
    );

    {
        let mut clients = clients.write().await;
        let mut client = if let Some(client) = clients.get(&id) {
            client.clone()
        } else {
            eprintln!("Client for user({}) disappeared!", id);
            return;
        };
        client.sender = Some(client_send_chan);
        clients.insert(id.clone(), client);
    }

    eprintln!("User({}) connected", id);

    while let Some(result) = client_recv_ws.next().await {
        let msg = match result {
            Ok(msg) => msg,
            Err(e) => {
                eprintln!("Error receiving ws msg for user({}): {:?}", id, e);
                break;
            }
        };
        handle_message(&id, msg, pool.clone(), &clients).await;
    }

    clients.write().await.remove(&id);
    eprintln!("User({}) disconnected", id);
}

pub async fn broadcast(msg: kapi::Response, clients: &Clients, with_state: ClientState) {
    broadcast_map(|_| msg.clone(), clients, with_state).await;
}

pub async fn broadcast_map<F>(f: F, clients: &Clients, with_state: ClientState)
where
    F: Fn(&kcore::player::PlayerId) -> kapi::Response,
{
    clients
        .read()
        .await
        .values()
        .filter(|client| client.state == with_state)
        .for_each(|client| {
            let msg = f(&client.user_data.id).clone();
            send(msg, client);
        });
}

pub fn send(msg: kapi::Response, client: &Client) {
    let msg = match msg.to_msg() {
        Ok(msg) => msg,
        Err(e) => {
            eprintln!(
                "Unable to encode message for user({}): {:?}",
                client.user_data.id, e
            );
            panic!();
        }
    };
    if let Some(sender) = &client.sender {
        match sender.send(Ok(msg)) {
            Ok(_) => (),
            Err(e) => eprintln!(
                "Unable to send message to user({}): {:?}",
                client.user_data.id, e
            ),
        }
    } else {
        eprintln!("No sender for user({})", client.user_data.id);
    }
}
