use serde::de::DeserializeOwned;
use serde::{Deserialize, Serialize};

use crate::core::action;
use crate::core::BoardType;
use crate::core::GameView;

use super::error::ClientError;
use super::model::game::GameId;
use super::model::pending_game::PendingGame;
use super::model::user::{UserData, UserId};
use super::result::Result;
use super::token::Token;

// rest API

pub trait RestRequest: DeserializeOwned + Serialize {
    fn path() -> &'static str;
}

pub trait RestResponse: DeserializeOwned + Serialize {
    fn from_str(res: &str) -> Result<Self> {
        serde_json::from_str(res).map_err(ClientError::SerdeError)
    }
}

#[derive(Debug, Deserialize, Serialize)]
pub struct RegisterRequest {
    pub username: String,
    pub password: String,
}

impl RestRequest for RegisterRequest {
    fn path() -> &'static str {
        "register"
    }
}

#[derive(Debug, Deserialize, Serialize)]
pub struct LoginRequest {
    pub username: String,
    pub password: String,
}

impl RestRequest for LoginRequest {
    fn path() -> &'static str {
        "login"
    }
}

#[derive(Debug, Deserialize, Serialize)]
pub struct LogoutRequest {
    pub user_id: UserId,
    pub token: Token,
}

impl RestRequest for LogoutRequest {
    fn path() -> &'static str {
        "logout"
    }
}

#[derive(Debug, Deserialize, Serialize)]
pub struct LoginResponse {
    pub user_id: UserId,
    pub token: Token,
}

impl RestResponse for LoginResponse {}

// ws API

pub trait MessageAdapter {
    fn to_str<'a>(&'a self) -> Result<&'a str>;
    fn from_string(msg: String) -> Self;
}

impl MessageAdapter for warp::ws::Message {
    fn to_str<'a>(&'a self) -> Result<&'a str> {
        self.to_str().map_err(|_| ClientError::WrongPayload)
    }

    fn from_string(msg: String) -> Self {
        warp::ws::Message::text(msg)
    }
}

impl MessageAdapter for String {
    fn to_str<'a>(&'a self) -> Result<&'a str> {
        Ok(self.as_str())
    }

    fn from_string(msg: String) -> Self {
        msg
    }
}

impl MessageAdapter for tokio_tungstenite::tungstenite::Message {
    fn to_str<'a>(&'a self) -> Result<&'a str> {
        match self {
            tokio_tungstenite::tungstenite::Message::Text(res) => Ok(res),
            _ => Err(ClientError::WrongPayload),
        }
    }

    fn from_string(msg: String) -> Self {
        tokio_tungstenite::tungstenite::Message::text(msg)
    }
}

pub trait Message {
    fn from_msg<M>(msg: M) -> Result<Self>
    where
        M: MessageAdapter,
        Self: DeserializeOwned + Sized,
    {
        serde_json::from_str(&msg.to_str()?).map_err(ClientError::SerdeError)
    }

    fn to_msg<M>(self) -> Result<M>
    where
        M: MessageAdapter,
        Self: Serialize + Sized,
    {
        serde_json::to_string(&self)
            .map(M::from_string)
            .map_err(ClientError::SerdeError)
    }
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct ClientErrorResponse {
    pub msg: String,
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct ServerErrorResponse {
    pub msg: String,
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct AuthenticateRequest {
    pub token: Token,
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub enum UnauthenticatedRequest {
    Authenticate(AuthenticateRequest),
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct CreateGameRequest {
    pub num_players: u8,
    pub victory_points_goal: u8,
    pub board_type: BoardType,
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct JoinGameRequest {
    pub game_id: GameId,
    //pub player_id: PlayerId,
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct StartGameRequest {
    pub game_id: GameId,
    //pub player_id: PlayerId,
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub enum LobbyRequest {
    CreateGame(CreateGameRequest),
    JoinGame(JoinGameRequest),
    StartGame(StartGameRequest),
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub enum LobbyResponse {
    Authenticated(Vec<UserData>),
    UserConnected(UserData),
    UserDisconnected(UserData),
    CreatedGame(PendingGame),
    PlayersChanged(PendingGame),
    GameStarted(GameId),
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct MakeMoveRequest {
    pub game_id: GameId,
    pub action_request: action::Request,
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub enum InGameRequest {
    MakeMove(MakeMoveRequest),
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub enum InGameResponse {
    MadeMove(action::Response),
    CurrentView(GameView),
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub enum Request {
    Unauthenticated(UnauthenticatedRequest),
    Lobby(LobbyRequest),
    InGame(InGameRequest),
}

impl Message for Request {}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub enum Response {
    Lobby(LobbyResponse),
    InGame(InGameResponse),
    ClientError(ClientErrorResponse),
    ServerError(ServerErrorResponse),
}

impl Message for Response {}
