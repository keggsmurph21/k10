use kserver;
use kserver::error::ClientError;
use kserver::kapi;
use kserver::kapi::Message as _;
use kserver::result::Result;
use rand::Rng;

async fn register(client: &reqwest::Client, req: kapi::RegisterRequest) -> Result<()> {
    client
        .post("http://localhost:3088/register")
        .body(serde_json::to_string(&req)?)
        .send()
        .await
        .map_err(ClientError::ReqwestError)?
        .error_for_status()?;
    Ok(())
}

async fn login(client: &reqwest::Client, req: kapi::LoginRequest) -> Result<kapi::LoginResponse> {
    let res = client
        .post("http://localhost:3088/login")
        .body(serde_json::to_string(&req)?)
        .send()
        .await
        .map_err(ClientError::ReqwestError)?
        .error_for_status()?;
    serde_json::from_str(&res.text().await?).map_err(ClientError::SerdeError)
}

struct User {
    pub id: kserver::model::user::UserId,
    pub token: kserver::token::Token,
}

impl User {
    /*
     */

    pub async fn register(username: String, password: String) -> Result<Self> {
        let client = reqwest::Client::new();

        register(
            &client,
            kapi::RegisterRequest {
                username: username.clone(),
                password: password.clone(),
            },
        )
        .await?;

        let login = login(
            &client,
            kapi::LoginRequest {
                username: username.clone(),
                password: password.clone(),
            },
        )
        .await?;

        println!("{:?}", login);

        Ok(Self {
            id: login.user_id,
            token: login.token,
        })
    }
}

use futures::SinkExt;
use tokio_tungstenite::tungstenite::Message;

pub async fn send<S>(
    writer: &mut futures_util::stream::SplitSink<S, Message>,
    req: kapi::Request,
) -> Result<()>
where
    S: futures::sink::Sink<Message>,
{
    let req: Message = req.to_msg()?;
    eprintln!("Sending {:?}", req);
    writer.send(req).await.unwrap_or_else(|_| panic!());
    Ok(())
}

fn random_username() -> String {
    let bytes = rand::thread_rng().gen::<[u8; 16]>();
    let uuid = uuid::Builder::from_bytes(bytes).build();
    uuid.to_hyphenated().to_string()
}

enum Event {
    Stdin(kapi::Request),
    Websocket(kapi::Response),
}

#[cfg(never)]
async fn try_main_old() -> Result<()> {
    let password: String = "hunter2".into();

    let alice = Client::connect(random_username(), password.clone()).await?;

    let (stdin_chan, read_chan) = mpsc::unbounded_channel();
    //let ws_chan = stdin_chan.clone();

    {
        tokio::task::spawn(async move {
            #[allow(unused)]
            loop {
                use kapi::Response::*;
                use kapi::*;

                let msg = match alice.recv().await {
                    Err(e) => {
                        eprintln!("Error reading from websocket: {:?}", e);
                        break;
                    }
                    Ok(msg) => msg,
                };

                eprintln!("Received {:?}", msg);

                let msg = match msg {
                    Lobby(LobbyResponse::Authenticated(users)) => {
                        eprintln!("Received unexpected message!");
                        panic!();
                    }
                    Lobby(LobbyResponse::UserConnected(user_data)) => {
                        // ignore it for now
                    }
                    Lobby(LobbyResponse::UserDisconnected(user_data)) => {
                        // ignore it for now
                    }
                    Lobby(LobbyResponse::CreatedGame(pending_game)) => {
                        panic!()
                    }
                    Lobby(LobbyResponse::PlayersChanged(pending_game)) => {
                        panic!()
                    }
                    Lobby(LobbyResponse::GameStarted(game_id)) => panic!(),
                    InGame(InGameResponse::MadeMove(res)) => panic!(),
                    InGame(InGameResponse::CurrentView(view)) => panic!(),
                    ClientError(err) => panic!(),
                    ServerError(err) => panic!(),
                };

                //ws_chan.send(event).unwrap_or_else(|_| panic!());
            }
        });
    }

    {
        tokio::task::spawn(async move {
            for line in std::io::stdin().lock().lines() {
                match line {
                    Err(e) => {
                        eprintln!("Error reading line: {:?}", e);
                        panic!();
                    }
                    Ok(line) => match kapi::Request::from_msg(line) {
                        Ok(req) => {
                            stdin_chan
                                .send(Event::Stdin(req))
                                .unwrap_or_else(|_| panic!());
                        }
                        Err(e) => {
                            eprintln!("{:?}", e);
                        }
                    },
                };
            }
        });
    }

    let mut stream = UnboundedReceiverStream::new(read_chan);
    loop {
        match stream.next().await {
            None => break,
            Some(Event::Stdin(event)) => println!("STDIN: {:?}", event),
            Some(Event::Websocket(event)) => println!("CHILD: {:?}", event),
        }
    }

    Ok(())
}

use futures_util::{future, pin_mut, StreamExt};
use std::io::BufRead;
use tokio::io::AsyncReadExt;
use tokio_tungstenite::connect_async;

async fn try_main() -> Result<()> {
    let (stdin_tx, stdin_rx) = futures_channel::mpsc::unbounded();
    tokio::spawn(async move {
        for msg in std::io::stdin().lock().lines() {
            match msg
                .map_err(ClientError::StdinError)
                .and_then(kapi::Request::from_msg)
                .and_then(|msg| msg.to_msg())
                .map(|msg| stdin_tx.unbounded_send(msg))
            {
                Ok(_) => {}
                Err(e) => eprintln!("Failed to write to channel: {:?}", e),
            };
        }
    });

    let password: String = "hunter2".into();
    let user = User::register(random_username(), password.clone()).await?;

    let url = format!("ws://localhost:3088/ws/{}", &user.id);
    let (ws_stream, _) = connect_async(url).await.expect("Failed to connect");
    //.map_err(ClientError::WebsocketError)?;

    /*
        client
            .send(i
            kapi::Request::Unauthenticated(
                kapi::UnauthenticatedRequest::Authenticate(kapi::AuthenticateRequest {
                    token: login.token,
                }),
            ))
            .await?;

        match client.recv().await {
            Ok(kapi::Response::Lobby(kapi::LobbyResponse::Authenticated(_))) => {}
            Ok(res) => {
                eprintln!("Unexpected response: {:?}", res);
                panic!();
            }
            Err(e) => {
                eprintln!("Unexpected error during auth: {:?}", e);
                panic!();
            }
        };

        Ok(client)
    }
    */

    let (mut writer, reader) = ws_stream.split();

    send(
        &mut writer,
        kapi::Request::Unauthenticated(kapi::UnauthenticatedRequest::Authenticate(
            kapi::AuthenticateRequest { token: user.token },
        )),
    )
    .await?;

    /*
    match recv(&mut reader).await {
        Ok(kapi::Response::Lobby(kapi::LobbyResponse::Authenticated(_))) => {}
        Ok(res) => {
            eprintln!("Unexpected response: {:?}", res);
            panic!();
        }
        Err(e) => {
            eprintln!("Unexpected error during auth: {:?}", e);
            panic!();
        }
    };
    */

    let stdin_to_ws = stdin_rx.map(Ok).forward(writer);
    let ws_to_stdout = {
        reader.for_each(|msg| async {
            let msg = kapi::Response::from_msg(msg.unwrap());

            let msg = match msg {
                Err(e) => {
                    eprintln!("Error reading from websocket: {:?}", e);
                    return;
                }
                Ok(msg) => msg,
            };

            eprintln!("Received {:?}", msg);

            use kapi::Response::*;
            use kapi::*;

            #[allow(unused)]
            let msg = match msg {
                Lobby(LobbyResponse::Authenticated(users)) => {
                    // TODO: Only handle this case if we've authenticated!
                    eprintln!("Authenticated: {:?}", users);
                }
                Lobby(LobbyResponse::UserConnected(user_data)) => {
                    // ignore it for now
                }
                Lobby(LobbyResponse::UserDisconnected(user_data)) => {
                    // ignore it for now
                }
                Lobby(LobbyResponse::CreatedGame(pending_game)) => {
                    // ignore it for now
                }
                Lobby(LobbyResponse::PlayersChanged(pending_game)) => {
                    // ignore it for now
                }
                Lobby(LobbyResponse::GameStarted(game_id)) => {
                    // ignore it for now
                }
                InGame(InGameResponse::MadeMove(res)) => {
                    // ignore it for now
                }
                InGame(InGameResponse::CurrentView(view)) => {
                    // ignore it for now
                }
                ClientError(err) => {
                    // ignore it for now
                }
                ServerError(err) => {
                    // ignore it for now
                }
            };
        })
    };

    pin_mut!(stdin_to_ws, ws_to_stdout);
    future::select(stdin_to_ws, ws_to_stdout).await;

    Ok(())
}

#[tokio::main]
async fn main() {
    try_main().await.unwrap();
}
