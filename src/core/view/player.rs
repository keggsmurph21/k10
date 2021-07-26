use serde::{Deserialize, Serialize};
use std::collections::HashSet;
use std::convert::TryFrom;

use super::super::action::Request;
use super::super::board::Index;
use super::super::dev_card::DevCardState;
use super::super::game::Game;
use super::super::player::Player;
use super::super::player::PlayerId;

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct MeView {
    player: Player,
    actions: Vec<Request>,
}

impl MeView {
    pub fn of(player: &Player, game: &Game) -> Self {
        Self {
            player: player.clone(),
            actions: player.available_actions(&game),
        }
    }
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct OtherView {
    id: PlayerId,
    num_resources: u8,
    public_victory_points: u8,
    cities: HashSet<Index>,
    roads: HashSet<Index>,
    settlements: HashSet<Index>,
    army_size: u8,
    num_dev_cards: u8,
    num_trades_offered_this_turn: u8,
    num_to_discard: u8,
    has_declined_trade: bool,
}

impl OtherView {
    pub fn of(player: &Player) -> Self {
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
                    .filter(|dc| dc.state != DevCardState::AlreadyPlayed)
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
