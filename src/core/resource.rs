use serde::{Deserialize, Serialize};

use std::collections::HashMap;

#[derive(Clone, Copy, Debug, Deserialize, Eq, Hash, PartialEq, Serialize)]
pub enum Resource {
    Brick,
    Ore,
    Sheep,
    Wheat,
    Wood,
}

impl Resource {
    pub fn all() -> Vec<Resource> {
        vec![
            Resource::Brick,
            Resource::Ore,
            Resource::Sheep,
            Resource::Wheat,
            Resource::Wood,
        ]
    }
}

pub type ResourceCounts = HashMap<Resource, u8>;
