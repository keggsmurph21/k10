use serde::{Deserialize, Serialize};

use super::super::board::Board;
use super::super::board::Index;
use super::super::game::Game;
use super::super::player::PlayerId;
use super::super::trade::Trade;
use super::player::{MeView, OtherView, PlayerView};

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct GameView {
    players: Vec<PlayerView>,
    current_player_index: u8,
    victory_points_goal: u8,
    num_dev_cards: usize,
    board: Board,
    robber_index: Index,
    turn_number: u32,
    has_rolled: bool,
    current_trade: Option<Trade>,
    has_largest_army: Option<PlayerId>,
    has_longest_road: Option<PlayerId>,
}

impl GameView {
    pub fn of(game: &Game, me: &PlayerId) -> Self {
        Self {
            players: game
                .player_order
                .iter()
                .cloned()
                .map(|id| {
                    let player = game.players.get(&id).unwrap();
                    if &id == me {
                        PlayerView::Me(MeView::of(&player, &game))
                    } else {
                        PlayerView::Other(OtherView::of(&player))
                    }
                })
                .collect(),
            current_player_index: game.current_player_index,
            victory_points_goal: game.victory_points_goal,
            num_dev_cards: game.dev_cards.len(),
            board: game.board.clone(),
            robber_index: game.robber_index,
            turn_number: game.turn_number,
            has_rolled: game.has_rolled,
            current_trade: game.current_trade.clone(),
            has_largest_army: game.has_largest_army,
            has_longest_road: game.has_longest_road,
        }
    }
}
