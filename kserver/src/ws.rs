use futures::{FutureExt, StreamExt};
use tokio::sync::mpsc;
use tokio_stream::wrappers::UnboundedReceiverStream;
use warp::ws::{Message, WebSocket};

use super::client::Clients;
use super::user::UserId;

pub async fn on_client_connection(ws: WebSocket, user_id: UserId, clients: Clients) {
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
        let client = clients.get(&user_id);
        // TODO: Gracefully fail
        let mut client = client.unwrap().clone();
        client.sender = Some(client_send_chan);
        clients.insert(user_id.clone(), client);
    }

    eprintln!("User({:?}) connected", user_id);

    while let Some(result) = client_recv_ws.next().await {
        let msg = match result {
            Ok(msg) => msg,
            Err(e) => {
                eprintln!("Error receiving ws msg for user({:?}): {:?}", user_id, e);
                break;
            }
        };
        handle_message(&user_id, msg, &clients).await;
    }

    clients.write().await.remove(&user_id);
    eprintln!("User({:?}) disconnected", user_id);
}

async fn handle_message(user_id: &UserId, msg: Message, clients: &Clients) {
    eprintln!("Message from user({}): {:?}", user_id, msg);
    let msg = if msg.is_close() {
        // TODO: Broadcast the fact that they left!
        None
    } else if msg.is_ping() {
        // Ignore it
        None
    } else if msg.is_text() {
        Some(msg.to_str().unwrap())
    } else if msg.is_binary() {
        // We don't handle this (yet?)
        panic!();
    } else {
        eprintln!("Unknown message: {:?}", msg);
        panic!();
    };

    // TODO: Parse the message!

    clients.read().await.values().for_each(|client| {
        // TODO: Filter by channel!
        if let Some(sender) = &client.sender {
            match sender.send(Ok(Message::text("hi"))) {
                Ok(_) => (),
                Err(e) => {
                    eprintln!(
                        "Unable to send message from user({}) => user({}): {:?}",
                        user_id, client.user_id, e
                    );
                    return;
                }
            }
        }
    });
}
