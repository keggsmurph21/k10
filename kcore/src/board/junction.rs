use serde::{Deserialize, Serialize};

use super::super::player::PlayerId;
use super::index::Index;
use super::port::Port;

#[derive(Debug, Deserialize, Eq, Hash, PartialEq, Serialize)]
pub struct Junction {
    pub index: Index,
    pub has_settlement: bool,
    pub has_city: bool,
    pub is_settleable: bool,
    pub port: Option<Port>,
    pub owner: Option<PlayerId>,
}

impl Junction {
    pub fn new(index: Index, port: Option<Port>) -> Junction {
        Junction {
            index,
            has_settlement: false,
            has_city: false,
            is_settleable: true,
            port,
            owner: None,
        }
    }
}
