use serde::{Deserialize, Serialize};

use super::super::player::PlayerId;
use super::index::Index;

#[derive(Clone, Debug, Deserialize, Eq, Hash, PartialEq, Serialize)]
pub struct Road {
    pub index: Index,
    pub owner: Option<PlayerId>,
}

impl Road {
    pub fn new(index: Index) -> Road {
        Road { index, owner: None }
    }
}
