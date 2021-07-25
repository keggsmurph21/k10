use serde::{Deserialize, Serialize};
use std::collections::HashSet;
use std::convert::TryFrom;

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct MeView {
    player: kcore::player::Player,
    actions: Vec<kcore::action::Request>,
}

impl MeView {
    pub fn of(player: &kcore::player::Player, game: &kcore::game::Game) -> Self {
        Self {
            player: player.clone(),
            actions: player.available_actions(&game),
        }
    }
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct OtherView {
    id: kcore::player::PlayerId,
    num_resources: u8,
    public_victory_points: u8,
    cities: HashSet<kcore::board::Index>,
    roads: HashSet<kcore::board::Index>,
    settlements: HashSet<kcore::board::Index>,
    army_size: u8,
    num_dev_cards: u8,
    num_trades_offered_this_turn: u8,
    num_to_discard: u8,
    has_declined_trade: bool,
}

impl OtherView {
    pub fn of(player: &kcore::player::Player) -> Self {
        Self {
            id: player.id,
            num_resources: u8::try_from(player.hand.values().filter(|count| count > &&0).count())
                .unwrap(),
            public_victory_points: player.victory_points.public,
            cities: player.cities.iter().cloned().collect(),
            roads: player.roads.iter().cloned().collect(),
            settlements: player.settlements.iter().cloned().collect(),
            army_size: player.army_size,
            num_dev_cards: u8::try_from(
                player
                    .dev_cards
                    .iter()
                    .filter(|dc| dc.state != kcore::dev_card::DevCardState::AlreadyPlayed)
                    .count(),
            )
            .unwrap(),
            num_trades_offered_this_turn: player.num_trades_offered_this_turn,
            num_to_discard: player.num_to_discard,
            has_declined_trade: player.has_declined_trade,
        }
    }
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub enum PlayerView {
    Me(MeView),
    Other(OtherView),
}
