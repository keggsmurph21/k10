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
    WebsocketError(#[from] tokio_tungstenite::tungstenite::Error),

    #[error("Error making request: {0}")]
    ReqwestError(#[from] reqwest::Error),

    #[error("Failed parsing data as utf-8: {0}")]
    Utf8Error(#[from] std::str::Utf8Error),

    #[error("Failed to write to channel")]
    ChannelWriteError(
        #[from] futures_channel::mpsc::TrySendError<tokio_tungstenite::tungstenite::Message>,
    ),

    #[error("Error during database I/O: {0}")]
    DatabaseError(#[from] sqlx::Error),

    #[error("Invalid value: {0}")]
    ValueError(&'static str),
}
