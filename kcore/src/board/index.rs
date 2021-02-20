use serde::{Deserialize, Serialize};

#[derive(Clone, Copy, Debug, Deserialize, Eq, Hash, PartialEq, Serialize)]
pub struct Index {
    pub x: u16,
    pub y: u16,
}

impl Index {
    pub fn new(x: u16, y: u16) -> Index {
        Index { x, y }
    }
}
