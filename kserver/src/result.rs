use super::error::ClientError;

pub type Result<T> = std::result::Result<T, ClientError>;
