use mpsc::UnboundedSender;
use std::collections::HashMap;
use std::result::Result;
use std::sync::Arc;
use tokio::sync::{mpsc, RwLock};
use warp::ws::Message;
use warp::Error;

use super::model::game::GameId;
use super::model::user::{User, UserData, UserId};
use super::token::{new_token, Token};

#[derive(Clone, Debug, Eq, PartialEq)]
pub enum ClientState {
    Unauthenticated,
    InLobby,
    InGame(GameId),
}

pub type Sender = UnboundedSender<Result<Message, Error>>;

#[derive(Clone, Debug)]
pub struct Client {
    pub user_data: UserData,
    pub state: ClientState,
    // NOTE: If we make this HashSet<Token>, we could allow multiple
    //       clients to be connected at a time. It's not clear if
    //       that's better, though ..
    pub token: Token,
    pub sender: Option<Sender>,
}

impl Client {
    pub fn new(user: &User) -> Client {
        Client {
            user_data: user.data(),
            token: new_token(),
            state: ClientState::Unauthenticated,
            sender: None,
        }
    }
}

pub type Clients = Arc<RwLock<HashMap<UserId, Client>>>;
