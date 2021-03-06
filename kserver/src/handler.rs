use serde::{Deserialize, Serialize};
use sqlx::sqlite::SqlitePool;
use std::result::Result;

use warp::http::StatusCode;
use warp::reply::json;
use warp::{Rejection, Reply};

use super::client::{Client, Clients};
use super::token::Token;
use super::user::{User, UserId};
use super::ws::on_client_connection;

pub async fn noop_handler() -> Result<impl Reply, Rejection> {
    Ok(StatusCode::OK)
}

#[derive(Debug, Deserialize)]
pub struct RegisterRequest {
    username: String,
    password: String,
}

async fn by_username(pool: &SqlitePool, username: &String) -> Result<Option<User>, sqlx::Error> {
    sqlx::query_as("SELECT * FROM users WHERE username = ?")
        .bind(username)
        .fetch_optional(pool)
        .await
}

async fn by_id(pool: &SqlitePool, id: &UserId) -> Result<User, sqlx::Error> {
    sqlx::query_as("SELECT * FROM users WHERE id = ?")
        .bind(id)
        .fetch_one(pool)
        .await
}

async fn insert_user(
    pool: &SqlitePool,
    username: &String,
    password: &String,
) -> Result<User, sqlx::Error> {
    let res = sqlx::query("INSERT INTO users (username, password) VALUES (?, ?)")
        .bind(username)
        .bind(password)
        .execute(pool)
        .await?;
    by_id(&pool, &res.last_insert_rowid()).await
}

pub async fn register_handler(
    body: RegisterRequest,
    pool: SqlitePool,
) -> Result<impl Reply, Rejection> {
    match by_username(&pool, &body.username).await {
        Ok(Some(_)) => Ok(StatusCode::BAD_REQUEST),
        Ok(None) => match insert_user(&pool, &body.username, &body.password).await {
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

#[derive(Debug, Deserialize)]
pub struct LoginRequest {
    username: String,
    password: String,
}

#[derive(Debug, Serialize)]
pub struct LoginResponse {
    user_id: UserId,
    token: Token,
}

pub async fn login_handler(
    body: LoginRequest,
    pool: SqlitePool,
    clients: Clients,
) -> Result<warp::reply::Response, Rejection> {
    match by_username(&pool, &body.username).await {
        Ok(Some(user)) => {
            if user.password != body.password {
                eprintln!("Invalid password for user {:?}", body.username);
                Ok(StatusCode::UNAUTHORIZED.into_response())
            } else {
                eprintln!("Logging in user {:?}", body.username);
                let client = Client::new(user.id);
                let token = client.token.clone();
                clients.write().await.insert(user.id, client);
                Ok(json(&LoginResponse {
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

#[derive(Debug, Deserialize)]
pub struct LogoutRequest {
    user_id: UserId,
    token: Token,
}

//#[must_use]
//fn validate_token(tokens: &HashMap<UserId, Token>, user_id: &UserId, token: &Token) -> bool {
//    match tokens.get(&user_id) {
//        None => false,
//        Some(stored_token) => stored_token == token,
//    }
//}

pub async fn logout_handler(
    body: LogoutRequest,
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
    token: Token, // TODO: Pass this somehow other than a path param!
    clients: Clients,
) -> Result<impl Reply, Rejection> {
    let client = clients.read().await.get(&user_id).cloned();
    match client {
        Some(client) => {
            if client.token != token {
                eprintln!("Token mismatch when logging out {:?}", user_id);
                Ok(StatusCode::BAD_REQUEST.into_response())
            } else {
                eprintln!("Upgrading to websocket for user({:?})", user_id);
                Ok(ws
                    .on_upgrade(move |socket| on_client_connection(socket, user_id, clients))
                    .into_response())
            }
        }
        None => Ok(StatusCode::NOT_FOUND.into_response()),
    }
}
