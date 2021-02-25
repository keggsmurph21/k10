use thiserror::Error;

#[derive(Error, Debug)]
pub enum ClientError {
    #[error("Error during de/serialization: {0}")]
    SerializationError(#[from] serde_json::error::Error),
    #[error("Error during database interop: {0}")]
    DatabaseError(#[from] sqlx::Error),
    #[error("Invalid value: {0}")]
    ValueError(&'static str),
}
