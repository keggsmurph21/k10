extern crate k10;

use k10::client::client::Client;

#[tokio::main]
async fn main() {
    Client::connect().await.unwrap();
}
