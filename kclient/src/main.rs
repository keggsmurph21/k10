mod client;
mod handler;
mod me;
mod rest;
mod state;

#[tokio::main]
async fn main() {
    client::Client::connect().await.unwrap();
}
