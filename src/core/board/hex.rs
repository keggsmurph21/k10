use serde::{Deserialize, Serialize};

use super::super::resource::Resource;
use super::index::Index;

#[derive(Clone, Debug, Deserialize, Eq, Hash, PartialEq, Serialize)]
pub struct HexResource {
    pub resource: Resource,
    pub roll: u8,
}

#[derive(Clone, Debug, Deserialize, Eq, Hash, PartialEq, Serialize)]
pub enum HexType {
    Desert,
    Ocean,
    WithResource(HexResource),
}

#[derive(Clone, Debug, Deserialize, Eq, Hash, PartialEq, Serialize)]
pub struct Hex {
    pub index: Index,
    pub hex_type: HexType,
}

impl Hex {
    pub fn new(index: Index, hex_type: HexType) -> Hex {
        Hex { index, hex_type }
    }
}
