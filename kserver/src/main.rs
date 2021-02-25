use sqlx::sqlite::SqlitePool;
use std::collections::HashMap;
use std::convert::Infallible; // TODO: Read more about this!
use std::sync::Arc; // TODO: Read more about this!
use tokio::sync::RwLock; // TODO: Read more about this!
use warp::Filter;

mod channel;
mod client;
mod handler;
mod request;
mod server;
mod token;
mod user;
mod ws;

use client::Clients;
use token::Token;
use user::UserId;

#[tokio::main]
async fn main() {
    let pool = SqlitePool::connect("sqlite::memory:").await.unwrap();

    sqlx::query(
        "CREATE TABLE users (
        username TEXT NOT NULL,
        password TEXT NOT NULL,
        id INTEGER NOT NULL PRIMARY KEY)",
    )
    .execute(&pool)
    .await
    .unwrap();

    let clients = Arc::new(RwLock::new(HashMap::new()));

    let noop_route = warp::path("noop").and_then(handler::noop_handler);

    let register_route = warp::path("register")
        .and(warp::post())
        .and(warp::body::json())
        .and(with_pool(pool.clone()))
        .and_then(handler::register_handler);

    let login_route = warp::path("login")
        .and(warp::post())
        .and(warp::body::json())
        .and(with_pool(pool.clone()))
        .and(with_clients(clients.clone()))
        .and_then(handler::login_handler);

    let logout_route = warp::path("logout")
        .and(warp::post())
        .and(warp::body::json())
        .and(with_clients(clients.clone()))
        .and_then(handler::logout_handler);

    let ws_route = warp::path("ws")
        .and(warp::ws())
        .and(warp::path::param::<UserId>())
        .and(warp::path::param::<Token>())
        .and(with_clients(clients.clone()))
        .and_then(handler::ws_handler);

    let routes = noop_route
        .or(register_route)
        .or(login_route)
        .or(logout_route)
        .or(ws_route)
        .with(warp::cors().allow_any_origin());

    warp::serve(routes).run(([0, 0, 0, 0], 3088)).await;
}

fn with_clients(clients: Clients) -> impl Filter<Extract = (Clients,), Error = Infallible> + Clone {
    warp::any().map(move || clients.clone())
}

fn with_pool(pool: SqlitePool) -> impl Filter<Extract = (SqlitePool,), Error = Infallible> + Clone {
    warp::any().map(move || pool.clone())
}

//fn main() {
//
//    let port = 3088;  // aka 0xc10
//    let bind_addr = format!("0.0.0.0:{}", port);
//    GameServer::new().serve(&bind_addr, &mut rng);
//}
