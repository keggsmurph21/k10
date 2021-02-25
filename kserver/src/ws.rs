use futures::{FutureExt, StreamExt};
use rand;
use serde::{Deserialize, Serialize};
use sqlx::SqlitePool;
use std::collections::HashSet;
use tokio::sync::mpsc;
use tokio_stream::wrappers::UnboundedReceiverStream;
use warp::ws::{Message, WebSocket};

use kcore::board::BoardType;

use super::client::{Client, ClientState, Clients};
use super::error::ClientError;
use super::game::{Game, GameId};
use super::game_view::GameView;
use super::pending_game::PendingGame;
use super::token::Token;
use super::user::{UserData, UserId};

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

#[derive(Debug, Serialize)]
struct BadRequestResponse {
    msg: String,
}

#[derive(Debug, Deserialize)]
struct AuthenticateRequest {
    token: Token,
}

impl AuthenticateRequest {
    #[must_use]
    pub fn from_msg(msg: Message) -> Result<Self, ClientError> {
        let msg = msg.to_str().unwrap_or("");
        let msg = serde_json::from_str::<Self>(msg)?;
        Ok(msg)
    }
}

#[derive(Serialize)]
struct AuthenticateReply {
    joined: bool,
    // NOTE: This will be None if authentication failed!
    users: Option<Vec<UserData>>,
}

async fn handle_unauthenticated(msg: Message, mut client: Client, clients: &Clients) {
    if msg.is_close() {
        // There's no one to broadcast this to if we haven't even
        // authenticated yet!
        return;
    }

    match AuthenticateRequest::from_msg(msg) {
        Err(e) => {
            eprintln!(
                "Couldn't parse message from user({}) as AuthenticateRequest: {:?}",
                client.user_data.id, e
            );
            send(
                BadRequestResponse {
                    msg: format!("{:?}", e),
                },
                &client,
            );
        }
        Ok(req) => {
            if req.token == client.token {
                send(
                    AuthenticateReply {
                        joined: true,
                        users: Some(
                            clients
                                .read()
                                .await
                                .values()
                                .filter(|client| client.state == ClientState::InLobby)
                                .map(|client| client.user_data.clone())
                                .collect(),
                        ),
                    },
                    &client,
                );
                broadcast(
                    LobbyBroadcast::UserConnected(client.user_data.clone()),
                    &clients,
                    ClientState::InLobby,
                )
                .await;

                client.state = ClientState::InLobby;
                clients.write().await.insert(client.user_data.id, client);
            } else {
                eprintln!("Auth failed for user({})", client.user_data.id);
                send(
                    AuthenticateReply {
                        joined: false,
                        users: None,
                    },
                    &client,
                );
            }
        }
    }
}

#[derive(Clone, Serialize)]
enum LobbyBroadcast {
    UserConnected(UserData),
    UserDisconnected(UserData),
    CreatedGame(PendingGame),
    PlayersChanged(PendingGame),
    GameStarted(GameId),
}

#[derive(Deserialize, Debug)]
struct CreateGameRequest {
    num_players: u8,
    victory_points_goal: u8,
    board_type: BoardType,
}

#[derive(Deserialize, Debug)]
struct JoinGameRequest {
    game_id: GameId,
}

#[derive(Deserialize, Debug)]
enum LobbyRequest {
    CreateGame(CreateGameRequest),
    JoinGame(JoinGameRequest),
}

impl LobbyRequest {
    #[must_use]
    pub fn from_msg(msg: Message) -> Result<Self, ClientError> {
        let msg = msg.to_str().unwrap_or("");
        let msg = serde_json::from_str::<Self>(msg)?;
        Ok(msg)
    }
}

async fn handle_create_game(
    req: CreateGameRequest,
    client: &Client,
    pool: SqlitePool,
) -> Result<PendingGame, ClientError> {
    if req.num_players < 2 {
        Err(ClientError::ValueError("Too few players: min=2"))
    } else if req.num_players > 5 {
        Err(ClientError::ValueError("Too many players: max=5"))
    } else if req.victory_points_goal < 8 {
        Err(ClientError::ValueError("Too few victory points: min=8"))
    } else if req.victory_points_goal > 12 {
        Err(ClientError::ValueError("Too many victory points: max=12"))
    } else {
        let mut game = PendingGame::new(
            &pool,
            req.num_players,
            req.victory_points_goal,
            req.board_type,
        )
        .await?;
        game.users.insert(client.user_data.id);
        game.clone().persist(&pool).await?;
        Ok(game)
    }
}

enum JoinedGame {
    Pending(PendingGame),
    Started(Game),
}

async fn handle_join_game(
    req: JoinGameRequest,
    client: &Client,
    pool: SqlitePool,
) -> Result<JoinedGame, ClientError> {
    let mut game = PendingGame::by_id(&pool, &req.game_id).await?;
    if game.users.len() == usize::from(game.num_players) {
        Err(ClientError::ValueError("Game already full!"))
    } else if game.users.contains(&client.user_data.id) {
        Err(ClientError::ValueError("Already in game!"))
    } else {
        game.users.insert(client.user_data.id);
        if game.users.len() == usize::from(game.num_players) {
            let id = game.id.clone();
            let users = game.users.iter().cloned().collect::<HashSet<UserId>>();
            let board_type = game.board_type.clone();
            let victory_points_goal = game.victory_points_goal.clone();
            game.delete(&pool).await?;
            let game = kcore::game::Game::new(
                users.iter().cloned().collect(),
                board_type,
                victory_points_goal,
                &mut rand::thread_rng(),
            )
            .unwrap();
            let game = Game::new(&pool, id, users, game).await?;
            Ok(JoinedGame::Started(game))
        } else {
            game.clone().persist(&pool).await?;
            Ok(JoinedGame::Pending(game))
        }
    }
}

#[derive(Serialize)]
struct GameStarted {
    view: GameView,
}

async fn handle_in_lobby(msg: Message, client: Client, pool: SqlitePool, clients: &Clients) {
    if msg.is_close() {
        broadcast(
            LobbyBroadcast::UserDisconnected(client.user_data.clone()),
            &clients,
            ClientState::InLobby,
        )
        .await;
        return;
    }

    match LobbyRequest::from_msg(msg) {
        Err(e) => {
            eprintln!(
                "Couldn't parse message from user({}): {:?}",
                client.user_data.id, e
            );
            send(
                BadRequestResponse {
                    msg: format!("{:?}", e),
                },
                &client,
            );
        }
        Ok(LobbyRequest::CreateGame(req)) => match handle_create_game(req, &client, pool).await {
            Ok(game) => {
                broadcast(
                    LobbyBroadcast::CreatedGame(game),
                    &clients,
                    ClientState::InLobby,
                )
                .await;
            }
            Err(e) => {
                println!(
                    "Couldn't create game for user({}): {:?}",
                    client.user_data.id, e
                );
                send(
                    BadRequestResponse {
                        msg: format!("{:?}", e),
                    },
                    &client,
                );
            }
        },
        Ok(LobbyRequest::JoinGame(req)) => match handle_join_game(req, &client, pool).await {
            Ok(JoinedGame::Started(game)) => {
                {
                    let mut clients = clients.write().await;
                    game.users.iter().for_each(|id| {
                        let mut client = clients.get(&id).unwrap().clone();
                        client.state = ClientState::InGame(game.id);
                        send(
                            GameStarted {
                                view: GameView::of(&game.game, &id),
                            },
                            &client,
                        );
                        clients.insert(*id, client);
                    });
                }
                broadcast(
                    LobbyBroadcast::GameStarted(game.id),
                    &clients,
                    ClientState::InLobby,
                )
                .await;
            }
            Ok(JoinedGame::Pending(game)) => {
                broadcast(
                    LobbyBroadcast::PlayersChanged(game),
                    &clients,
                    ClientState::InLobby,
                )
                .await;
            }
            Err(e) => {
                println!(
                    "Couldn't join game for user({}): {:?}",
                    client.user_data.id, e
                );
                send(
                    BadRequestResponse {
                        msg: format!("{:?}", e),
                    },
                    &client,
                );
            }
        },
    }
}

async fn handle_in_game(_msg: Message, _client: Client, _clients: &Clients) {
    panic!();
}

async fn handle_message(id: &UserId, msg: Message, pool: SqlitePool, clients: &Clients) {
    eprintln!("Message from user({}): {:?}", id, msg);
    let client = match clients.read().await.get(&id).cloned() {
        Some(client) => client,
        None => {
            eprintln!("Client for user({}) disappeared!", id);
            return;
        }
    };

    match client.state {
        ClientState::Unauthenticated => handle_unauthenticated(msg, client, clients).await,
        ClientState::InLobby => handle_in_lobby(msg, client, pool, clients).await,
        ClientState::InGame(_game) => handle_in_game(msg, client, clients).await,
    };
}

async fn broadcast<M: Serialize + Clone>(msg: M, clients: &Clients, with_state: ClientState) {
    clients
        .read()
        .await
        .values()
        .filter(|client| client.state == with_state)
        .for_each(|client| {
            send(msg.clone(), client);
        });
}

fn send<M: Serialize>(msg: M, client: &Client) {
    let msg = match serde_json::to_string(&msg) {
        Ok(msg) => Message::text(msg),
        Err(e) => {
            eprintln!(
                "Unable to encode message for user({}): {:?}",
                client.user_data.id, e
            );
            return;
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
