use crate::server::api;
use crate::server::model::user::UserId;
use crate::server::result::Result;
use crate::server::token::Token;
use rand::Rng;

use super::rest;

/// Registration details for connecting to the server
/// for this user.
#[derive(Debug)]
pub struct Me {
    pub username: String,
    pub id: UserId,
    pub token: Token,
}

impl Me {
    pub async fn register(username: String, password: String) -> Result<Self> {
        let client = reqwest::Client::new();

        rest::register(
            &client,
            api::RegisterRequest {
                username: username.clone(),
                password: password.clone(),
            },
        )
        .await?;

        let login = rest::login(
            &client,
            api::LoginRequest {
                username: username.clone(),
                password: password.clone(),
            },
        )
        .await?;

        eprintln!("Registered {:?}", login);

        Ok(Self {
            username,
            id: login.user_id,
            token: login.token,
        })
    }

    fn random_username() -> String {
        let bytes = rand::thread_rng().gen::<[u8; 16]>();
        let uuid = uuid::Builder::from_bytes(bytes).build();
        uuid.to_hyphenated().to_string()
    }

    pub async fn random() -> Result<Self> {
        let username = Self::random_username();
        let password = String::from("hunter2");
        Self::register(username, password).await
    }
}
