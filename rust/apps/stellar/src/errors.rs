use alloc::format;
use alloc::string::{String, ToString};
use keystore::errors::KeystoreError;
use third_party::hex;
use third_party::serde_json;
use third_party::serde_json::Error;
use third_party::thiserror;
use thiserror::Error;

#[derive(Error, Debug)]
pub enum StellarError {
    #[error("meet error when encoding address: {0}")]
    AddressError(String),
    #[error("keystore operation failed, reason: {0}")]
    KeystoreError(String),
    #[error("Meet invalid data when reading `{0}`")]
    InvalidData(String),
    #[error("Could not parse transaction, reason: `{0}`")]
    ParseTxError(String),
}

pub type Result<T> = core::result::Result<T, StellarError>;

impl From<KeystoreError> for StellarError {
    fn from(value: KeystoreError) -> Self {
        Self::KeystoreError(value.to_string())
    }
}

impl From<hex::FromHexError> for StellarError {
    fn from(value: hex::FromHexError) -> Self {
        Self::InvalidData(format!("hex operation failed {}", value))
    }
}

impl From<serde_json::Error> for StellarError {
    fn from(value: Error) -> Self {
        Self::ParseTxError(format!(
            "serde json operation failed {:?}",
            value.to_string()
        ))
    }
}
