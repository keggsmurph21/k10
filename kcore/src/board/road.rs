use serde::{Deserialize, Serialize};

use super::super::player::PlayerId;
use super::index::Index;

#[derive(Debug, Deserialize, Eq, Hash, PartialEq, Serialize)]
pub enum RoadOrientation {
    BackSlash,
    ForwardSlash,
    Pipe,
}

#[derive(Debug, Deserialize, Eq, Hash, PartialEq, Serialize)]
pub struct Road {
    pub index: Index,
    pub orientation: RoadOrientation,
    pub owner: Option<PlayerId>,
}

impl Road {
    pub fn new(index: Index, orientation: RoadOrientation) -> Road {
        Road {
            index,
            orientation,
            owner: None,
        }
    }
}
