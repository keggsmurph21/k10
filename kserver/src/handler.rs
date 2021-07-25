use rand;
use sqlx::sqlite::SqlitePool;
use std::collections::HashSet;
use std::result::Result;

use warp::http::StatusCode;
use warp::reply::json;
use warp::ws::Message;
use warp::{Rejection, Reply};

use super::client::{Client, ClientState, Clients};
use super::error::ClientError;
use super::kapi;
use super::kapi::Message as _;
use super::model::game::Game;
use super::model::game::GameId;
use super::model::pending_game::PendingGame;
use super::model::user::{User, UserId};
use super::view::game::GameView;
use super::ws;

pub async fn noop_handler() -> Result<impl Reply, Rejection> {
    Ok(StatusCode::OK)
}

pub async fn register_handler(
    body: kapi::RegisterRequest,
    pool: SqlitePool,
) -> Result<impl Reply, Rejection> {
    match User::by_username(&pool, &body.username).await {
        Ok(Some(_)) => Ok(StatusCode::BAD_REQUEST),
        Ok(None) => match User::new(&pool, &body.username, &body.password).await {
            Ok(user) => {
                eprintln!(
                    "Inserted user({}) with username {:?}",
                    user.id, user.username
                );
                Ok(StatusCode::CREATED)
            }
            Err(e) => {
                eprintln!("Unable to insert user: {:?}", e);
                Ok(StatusCode::INTERNAL_SERVER_ERROR)
            }
        },
        Err(e) => {
            eprintln!("Unable to fetch data: {:?}", e);
            Ok(StatusCode::INTERNAL_SERVER_ERROR)
        }
    }
}

pub async fn login_handler(
    body: kapi::LoginRequest,
    pool: SqlitePool,
    clients: Clients,
) -> Result<impl Reply, Rejection> {
    match User::by_username(&pool, &body.username).await {
        Ok(Some(user)) => {
            if user.password != body.password {
                eprintln!("Invalid password for user {:?}", body.username);
                Ok(StatusCode::UNAUTHORIZED.into_response())
            } else {
                eprintln!("Logging in user {:?}", body.username);
                let client = Client::new(&user);
                let token = client.token.clone();
                clients.write().await.insert(user.id, client);
                Ok(json(&kapi::LoginResponse {
                    user_id: user.id,
                    token,
                })
                .into_response())
            }
        }
        Ok(None) => {
            eprintln!("Unable to find user {:?}", body.username);
            Ok(StatusCode::BAD_REQUEST.into_response())
        }
        Err(e) => {
            eprintln!("Unable to fetch data: {:?}", e);
            Ok(StatusCode::INTERNAL_SERVER_ERROR.into_response())
        }
    }
}

pub async fn logout_handler(
    body: kapi::LogoutRequest,
    clients: Clients,
) -> Result<impl Reply, Rejection> {
    let mut clients = clients.write().await;
    match clients.get(&body.user_id) {
        None => {
            eprintln!("User {:?} is not logged in!", body.user_id);
            Ok(StatusCode::NOT_FOUND)
        }
        Some(client) => {
            if client.token != body.token {
                eprintln!("Token mismatch when logging out {:?}", body.user_id);
                Ok(StatusCode::BAD_REQUEST)
            } else {
                eprintln!("Logging out {:?}", body.user_id);
                clients.remove(&body.user_id);
                Ok(StatusCode::NO_CONTENT)
            }
        }
    }
}

pub async fn ws_handler(
    ws: warp::ws::Ws,
    user_id: UserId,
    pool: SqlitePool,
    clients: Clients,
) -> Result<impl Reply, Rejection> {
    if clients.read().await.contains_key(&user_id) {
        eprintln!("Attempting ws upgrade user({})", user_id);
        Ok(ws
            .on_upgrade(move |socket| ws::on_client_connection(socket, user_id, pool, clients))
            .into_response())
    } else {
        eprintln!("No client found for user({})", user_id);
        Ok(StatusCode::NOT_FOUND.into_response())
    }
}

async fn handle_unauthenticated(
    req: kapi::UnauthenticatedRequest,
    mut client: Client,
    clients: &Clients,
) {
    match req {
        kapi::UnauthenticatedRequest::Authenticate(req) => {
            if req.token == client.token {
                let users = clients
                    .read()
                    .await
                    .values()
                    .filter(|client| client.state == ClientState::InLobby)
                    .map(|client| client.user_data.clone())
                    .collect();

                let res = kapi::LobbyResponse::Authenticated(users);
                let res = kapi::Response::Lobby(res);
                ws::send(res, &client);

                let res = kapi::LobbyResponse::UserConnected(client.user_data.clone());
                let res = kapi::Response::Lobby(res);
                ws::broadcast(res, &clients, ClientState::InLobby).await;

                client.state = ClientState::InLobby;
                clients.write().await.insert(client.user_data.id, client);
            } else {
                eprintln!("Auth failed for user({})", client.user_data.id);
                let res = kapi::ClientErrorResponse {
                    msg: "Authentication failed".into(),
                };
                ws::send(kapi::Response::ClientError(res), &client);
            }
        }
    }
}

async fn handle_create_game(
    req: kapi::CreateGameRequest,
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
    req: kapi::JoinGameRequest,
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

async fn handle_in_lobby(
    req: kapi::LobbyRequest,
    client: Client,
    pool: SqlitePool,
    clients: &Clients,
) {
    match req {
        kapi::LobbyRequest::CreateGame(req) => match handle_create_game(req, &client, pool).await {
            Ok(game) => {
                eprintln!("Created game with id {}", game.id);
                let res = kapi::LobbyResponse::CreatedGame(game);
                let res = kapi::Response::Lobby(res);
                ws::broadcast(res, &clients, ClientState::InLobby).await;
            }
            Err(e) => {
                println!(
                    "Couldn't create game for user({}): {:?}",
                    client.user_data.id, e
                );
                ws::send(
                    kapi::Response::ServerError(kapi::ServerErrorResponse {
                        msg: format!("{:?}", e),
                    }),
                    &client,
                );
            }
        },
        kapi::LobbyRequest::JoinGame(req) => match handle_join_game(req, &client, pool).await {
            Ok(JoinedGame::Started(game)) => {
                {
                    let mut clients = clients.write().await;
                    game.users.iter().for_each(|id| {
                        let mut client = clients.get(&id).unwrap().clone();
                        client.state = ClientState::InGame(game.id);
                        let res = kapi::InGameResponse::CurrentView(GameView::of(&game.game, &id));
                        let res = kapi::Response::InGame(res);
                        ws::send(res, &client);
                        clients.insert(*id, client);
                    });
                }
                let res = kapi::LobbyResponse::GameStarted(game.id);
                let res = kapi::Response::Lobby(res);
                ws::broadcast(res, &clients, ClientState::InLobby).await;
            }
            Ok(JoinedGame::Pending(game)) => {
                let res = kapi::LobbyResponse::PlayersChanged(game);
                let res = kapi::Response::Lobby(res);
                ws::broadcast(res, &clients, ClientState::InLobby).await;
            }
            Err(e) => {
                println!(
                    "Couldn't join game for user({}): {:?}",
                    client.user_data.id, e
                );
                ws::send(
                    // TODO: This isn't always a server error!
                    kapi::Response::ServerError(kapi::ServerErrorResponse {
                        msg: format!("{:?}", e),
                    }),
                    &client,
                );
            }
        },
    }
}

async fn handle_in_game(
    req: kapi::InGameRequest,
    game_id: GameId,
    client: Client,
    pool: SqlitePool,
    clients: &Clients,
) {
    let mut game = Game::by_id(&pool, &game_id).await.unwrap();
    let player_id = &client.user_data.id;

    match req {
        kapi::InGameRequest::MakeMove(req) => {
            let res = game
                .game
                .make_move(*player_id, req, &mut rand::thread_rng());
            eprintln!("{:?}", res);
            let res = kapi::InGameResponse::MadeMove(res);
            let res = kapi::Response::InGame(res);
            ws::send(res, &client);

            ws::broadcast_map(
                |player_id| {
                    let view = GameView::of(&game.game, player_id);
                    let res = kapi::InGameResponse::CurrentView(view);
                    let res = kapi::Response::InGame(res);
                    res
                },
                &clients,
                ClientState::InGame(game_id),
            )
            .await;

            game.persist(&pool).await.unwrap();
        }
    }
}

pub async fn handle_message(id: &UserId, msg: Message, pool: SqlitePool, clients: &Clients) {
    eprintln!("Message from user({}): {:?}", id, msg);
    let client = match clients.read().await.get(&id).cloned() {
        Some(client) => client,
        None => {
            eprintln!("Client for user({}) disappeared!", id);
            return;
        }
    };

    if msg.is_close() {
        match &client.state {
            ClientState::Unauthenticated => {} // there's no one to tell!
            state => {
                let res = kapi::LobbyResponse::UserDisconnected(client.user_data.clone());
                let res = kapi::Response::Lobby(res);
                ws::broadcast(res, &clients, state.clone()).await;
            }
        }
        return;
    }

    match (&client.state, kapi::Request::from_msg(msg)) {
        (_, Err(e)) => {
            let res = format!(
                "Unable to parse request from {}: {:?}",
                client.user_data.id, e
            );
            eprintln!("{}", res);
            ws::send(
                kapi::Response::ClientError(kapi::ClientErrorResponse { msg: res }),
                &client,
            );
        }
        (ClientState::Unauthenticated, Ok(kapi::Request::Unauthenticated(req))) => {
            handle_unauthenticated(req, client, clients).await
        }
        (ClientState::InLobby, Ok(kapi::Request::Lobby(req))) => {
            handle_in_lobby(req, client, pool, clients).await
        }
        (ClientState::InGame(game_id), Ok(kapi::Request::InGame(req))) => {
            handle_in_game(req, *game_id, client, pool, clients).await
        }
        (_, req) => {
            let res = format!(
                "Invalid request from {} in {:?}: {:?}",
                client.user_data.id, client.state, req
            );
            eprintln!("{}", res);
            ws::send(
                kapi::Response::ClientError(kapi::ClientErrorResponse { msg: res }),
                &client,
            );
        }
    }
}
