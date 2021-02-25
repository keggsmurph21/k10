use mpsc::UnboundedSender;
use std::collections::{HashMap, HashSet};
use std::result::Result;
use std::sync::Arc;
use tokio::sync::{mpsc, RwLock};
use warp::ws::Message;
use warp::Error;

use super::channel::Channel;
use super::token::{new_token, Token};
use super::user::UserId;

#[derive(Clone, Debug)]
pub struct Client {
    pub user_id: UserId,
    // NOTE: If we make this HashSet<Token>, we could allow multiple
    //       clients to be connected at a time. It's not clear if
    //       that's better, though ..
    pub token: Token,
    pub channels: HashSet<Channel>,
    pub sender: Option<UnboundedSender<Result<Message, Error>>>,
}

impl Client {
    pub fn new(user_id: UserId) -> Client {
        Client {
            user_id,
            token: new_token(),
            channels: vec![Channel::Lobby].into_iter().collect(),
            sender: None,
        }
    }
}

pub type Clients = Arc<RwLock<HashMap<UserId, Client>>>;
