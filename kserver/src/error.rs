use thiserror::Error;

#[derive(Error, Debug)]
pub enum ClientError {
    // Use this when it's serde's internal error
    #[error("Error during de/serialization: {0}")]
    SerdeError(#[from] serde_json::error::Error),

    // Use this when it's any serialization error *not*
    // directly from the serde API.
    #[error("Error during websocket I/O: Non-text payload")]
    WrongPayload,

    #[error("Error during websocket I/O: {0}")]
    WebsocketError(#[from] websocket::WebSocketError),

    #[error("Error connecting to websocket host: {0}")]
    WebsocketConnectionError(#[from] websocket::client::ParseError),

    #[error("Error making request: {0}")]
    ReqwestError(#[from] reqwest::Error),

    #[error("Failed reading data from stdin")]
    StdinError(#[from] std::io::Error),

    #[error("Error during database I/O: {0}")]
    DatabaseError(#[from] sqlx::Error),

    #[error("Invalid value: {0}")]
    ValueError(&'static str),
}
