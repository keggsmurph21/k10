use serde::Serialize;
use std::collections::HashSet;
use std::convert::TryFrom;

#[derive(Debug, Serialize)]
pub struct GameView {
    players: Vec<PlayerView>,
    current_player_index: u8,
    victory_points_goal: u8,
    num_dev_cards: usize,
    board: Option<kcore::board::Board>, // TODO: impl Clone for Board {}
    robber_index: kcore::board::Index,
    turn_number: u32,
    has_rolled: bool,
    current_trade: Option<kcore::trade::Trade>,
    has_largest_army: Option<kcore::player::PlayerId>,
    has_longest_road: Option<kcore::player::PlayerId>,
}

impl GameView {
    pub fn of(game: &kcore::game::Game, me: &kcore::player::PlayerId) -> Self {
        Self {
            players: game
                .player_order
                .iter()
                .cloned()
                .map(|id| {
                    let player = game.players.get(&id).unwrap();
                    if &id == me {
                        //PlayerView::Me(MeView::of(&player))
                        PlayerView::Me(None)
                    } else {
                        PlayerView::Other(OtherView::of(&player))
                    }
                })
                .collect(),
            current_player_index: game.current_player_index,
            victory_points_goal: game.victory_points_goal,
            num_dev_cards: game.dev_cards.len(),
            board: None,
            robber_index: game.robber_index,
            turn_number: game.turn_number,
            has_rolled: game.has_rolled,
            current_trade: None, // TODO: impl Clone for Trade
            has_largest_army: game.has_largest_army,
            has_longest_road: game.has_longest_road,
        }
    }
}

#[derive(Debug, Serialize)]
pub struct MeView {
    player: kcore::player::Player,
    actions: Vec<kcore::action::Request>,
}

impl MeView {
    pub fn of(player: &kcore::player::Player) -> Self {
        panic!();
        //Self { player, actions }
    }
}

#[derive(Debug, Serialize)]
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

#[derive(Debug, Serialize)]
pub enum PlayerView {
    Me(Option<MeView>),
    Other(OtherView),
}
