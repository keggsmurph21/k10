use serde::{Deserialize, Serialize};

use kcore::action;

use super::user::{Password, UserId, UserToken, Username};

pub type GameId = u64;

#[derive(Debug, Deserialize, Serialize)]
pub struct GameOptions {}

pub type Message = String;

#[derive(Debug, Deserialize, Serialize)]
pub enum Request {
    Register(Username, Password),
    Login(Username, Password),
    NewGame(UserId, GameId, GameOptions),
    JoinGame(UserId, GameId),
    LeaveGame(UserId, GameId),
    MakeMove(UserId, UserToken, action::Request),
    Chat(Message),
}

pub type ErrorMessage = String;

#[derive(Debug, Deserialize, Serialize)]
pub enum Response {
    Register(UserId),
    Login(UserId),
    NewGame(GameId),
    JoinGame(bool),
    LeaveGame(bool),
    MakeMove(action::Response),
    Chat, // ack
    InvalidRequest(ErrorMessage),
}
