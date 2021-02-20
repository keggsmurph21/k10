use serde::{Deserialize, Serialize};

use super::super::resource::Resource;

#[derive(Clone, Copy, Debug, Deserialize, Eq, Hash, PartialEq, Serialize)]
pub enum Port {
    Any,
    Only(Resource),
}
