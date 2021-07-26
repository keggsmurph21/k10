use serde::{Deserialize, Serialize};

#[derive(Clone, Copy, Debug, Deserialize, Eq, Hash, PartialEq, Serialize)]
pub enum DevCard {
    Knight,
    VictoryPoint,
    Monopoly,
    RoadBuilding,
    YearOfPlenty,
}

#[derive(Clone, Copy, Debug, Deserialize, Eq, Hash, PartialEq, Serialize)]
pub enum DevCardState {
    AlreadyPlayed,
    CanPlay,
    CannotPlay,
}

#[derive(Clone, Copy, Debug, Deserialize, Serialize)]
pub struct DevCardInHand {
    pub card: DevCard,
    pub state: DevCardState,
}

impl DevCardInHand {
    pub fn new(card: DevCard) -> DevCardInHand {
        let state = match card {
            DevCard::VictoryPoint => DevCardState::CanPlay,
            _ => DevCardState::CannotPlay,
        };
        DevCardInHand { card, state }
    }
}
