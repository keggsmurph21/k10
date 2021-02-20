use serde::{Deserialize, Serialize};

use std::collections::HashSet;

use super::player::PlayerId;
use super::resource::ResourceCounts;

#[derive(Debug, Deserialize, PartialEq, Serialize)]
pub enum TradeWith {
    Bank,
    Players(HashSet<PlayerId>),
}

#[derive(Debug, Deserialize, PartialEq, Serialize)]
pub struct Trade {
    pub giving: ResourceCounts,
    pub getting: ResourceCounts,
    pub with: TradeWith,
}

impl Trade {
    pub fn new(giving: ResourceCounts, getting: ResourceCounts, with: TradeWith) -> Trade {
        Trade {
            giving,
            getting,
            with,
        }
    }

    pub fn empty() -> Trade {
        Trade::new(
            ResourceCounts::new(),
            ResourceCounts::new(),
            TradeWith::Players(HashSet::new()),
        )
    }
}
