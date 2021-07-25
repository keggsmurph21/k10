use serde::{Deserialize, Serialize};

use std::collections::HashSet;

use super::action::Request;
use super::board::{boards, Board, Index, Junction, Port, Road};
use super::dev_card::{DevCard, DevCardInHand, DevCardState};
use super::game::Game;
use super::resource::{Resource, ResourceCounts};
use super::trade::{Trade, TradeWith};

pub type PlayerId = i64;

#[derive(Clone, Debug, Deserialize, Serialize)]
pub enum PlayerState {
    AfterBuildingFreeSettlement,
    AfterRollingSeven,
    ChooseInitialResources,
    Root,
    Stealing,
    WaitForTurn,
    WaitForTradeResponses,
}

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct VictoryPoints {
    pub public: u8,
    pub private: u8,
}

impl VictoryPoints {
    fn new() -> VictoryPoints {
        VictoryPoints {
            public: 0,
            private: 0,
        }
    }
}

const MAX_TRADES_PER_TURN: u8 = u8::MAX - 1;
const DEFAULT_BANK_TRADE_RATE: u8 = 4;
const GENERIC_PORT_BANK_TRADE_RATE: u8 = 3;
const SPECIALTY_PORT_BANK_TRADE_RATE: u8 = 2;

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct Player {
    pub id: PlayerId,
    pub state: PlayerState,
    pub hand: ResourceCounts,
    pub victory_points: VictoryPoints,
    pub cities: HashSet<Index>,
    pub roads: HashSet<Index>,
    pub settlements: HashSet<Index>,
    pub army_size: u8,
    pub dev_cards: Vec<DevCardInHand>,
    pub has_played_dev_card: bool,
    pub num_trades_offered_this_turn: u8,
    pub num_to_discard: u8,
    pub has_declined_trade: bool,
    pub bank_trade_rates: ResourceCounts,
}

impl Player {
    pub fn new(id: PlayerId) -> Player {
        Player {
            id,
            state: PlayerState::WaitForTurn,
            hand: ResourceCounts::new(),
            victory_points: VictoryPoints::new(),
            cities: HashSet::new(),
            roads: HashSet::new(),
            settlements: HashSet::new(),
            army_size: 0,
            dev_cards: Vec::new(),
            has_played_dev_card: false,
            num_trades_offered_this_turn: 0,
            num_to_discard: 0,
            has_declined_trade: false,
            bank_trade_rates: Resource::all()
                .into_iter()
                .map(|resource| (resource, DEFAULT_BANK_TRADE_RATE))
                .collect(),
        }
    }

    fn count_resource(&self, resource: Resource, counts: &ResourceCounts) -> (Resource, u8, u8) {
        let have = self.hand.get(&resource).unwrap_or(&0);
        let count = counts.get(&resource).unwrap_or(&0);
        (resource, *have, *count)
    }

    pub fn can_afford(&self, cost: &ResourceCounts) -> bool {
        Resource::all()
            .into_iter()
            .map(|r| self.count_resource(r, cost))
            .map(|(_, have, giving)| have >= giving)
            .fold(true, |acc, b| acc && b)
    }

    pub fn spend(&mut self, cost: &ResourceCounts) {
        self.hand = Resource::all()
            .into_iter()
            .map(|r| self.count_resource(r, cost))
            .map(|(resource, have, giving)| {
                if have < giving {
                    // it's a 🐞!
                    eprintln!(
                        "Player({}): Spending {:?} but hand contains only {:?}",
                        self.id, cost, self.hand
                    );
                    panic!();
                }
                (resource, have - giving)
            })
            .collect();
    }

    pub fn accrue(&mut self, windfall: &ResourceCounts) {
        self.hand = Resource::all()
            .into_iter()
            .map(|r| self.count_resource(r, windfall))
            .map(|(resource, have, giving)| (resource, have + giving))
            .collect()
    }

    pub fn bank_trade_rate(&self, resource: &Resource) -> u8 {
        *self.bank_trade_rates.get(&resource).unwrap()
    }

    fn map_dev_cards<F>(&self, f: F) -> Vec<DevCardInHand>
    where
        F: FnMut(&DevCardInHand) -> DevCardInHand,
    {
        self.dev_cards.iter().map(f).collect()
    }

    pub fn reset_for_new_turn(&mut self) {
        self.num_trades_offered_this_turn = 0;
        self.dev_cards = self.map_dev_cards(|dev_card| match *dev_card {
            DevCardInHand {
                card,
                state: DevCardState::CannotPlay,
            } => DevCardInHand {
                card,
                state: DevCardState::CanPlay,
            },
            card => card,
        });
        self.has_played_dev_card = false;
        self.has_declined_trade = false;
    }

    pub fn use_dev_card(&mut self, used_dev_card: &DevCard) {
        // Mark this card as "used"
        let mut has_made_update = false;
        self.dev_cards = self.map_dev_cards(|dev_card| match *dev_card {
            DevCardInHand {
                card,
                state: DevCardState::CanPlay,
            } if !has_made_update && &card == used_dev_card => {
                has_made_update = true;
                DevCardInHand {
                    card,
                    state: DevCardState::AlreadyPlayed,
                }
            }
            card => card,
        });
        if !has_made_update {
            // it's a 🐞!
            eprintln!(
                "Player({}): Using {:?} but only have {:?}",
                self.id, used_dev_card, self.dev_cards
            );
            panic!();
        }
        self.has_played_dev_card = true;
    }

    pub fn push_city(&mut self, junction: &Junction) {
        self.cities.insert(junction.index);
    }

    pub fn push_dev_card(&mut self, dev_card: DevCard) {
        self.dev_cards.push(DevCardInHand::new(dev_card));
    }

    pub fn push_road(&mut self, road: &Road) {
        self.roads.insert(road.index);
    }

    pub fn push_settlement(&mut self, junction: &Junction) {
        self.settlements.insert(junction.index);
        self.bank_trade_rates = Resource::all()
            .into_iter()
            .map(|resource| {
                let current = *self.bank_trade_rates.get(&resource).unwrap();
                let rate = match junction.port {
                    Some(Port::Only(specialty)) if resource == specialty => {
                        SPECIALTY_PORT_BANK_TRADE_RATE
                    }
                    Some(Port::Any) => std::cmp::min(current, GENERIC_PORT_BANK_TRADE_RATE),
                    _ => current,
                };
                (resource, rate)
            })
            .collect();
    }

    pub fn longest_road(&self, board: &Board) -> u8 {
        board.longest_road(self.id, &self.roads)
    }

    fn reachable_from(&self, road: &Index, board: &Board) -> HashSet<Index> {
        board
            .junction_neighbors_of_road(road)
            .into_iter()
            .map(|j| &j.index)
            .filter(|j| board.can_build_road_thru(&self.id, j))
            .map(|j| board.road_neighbors_of_junction(j).into_iter())
            .flatten()
            .filter(|r| r.owner.is_none())
            .map(|r| r.index)
            .collect()
    }

    pub fn reachable_roads(&self, board: &Board) -> HashSet<Index> {
        self.roads
            .iter()
            .map(|r| self.reachable_from(r, board))
            .flatten()
            .chain(
                self.settlements
                    .iter()
                    .map(|j| board.road_neighbors_of_junction(j).into_iter())
                    .flatten()
                    .filter(|r| r.owner.is_none())
                    .map(|r| r.index),
            )
            .collect()
    }

    fn doubly_reachable_roads(&self, board: &Board) -> HashSet<(Index, Index)> {
        // For road building, we can build two roads.  The second road
        // is deemed legal *if it would be legal after playing the first
        // road*.  That means we have to calculate not only the pairs
        // that are legal *now*, but also the pairs that will be legal
        // *in the future*, assuming we make a legal first move.
        let reachable = self.reachable_roads(&board);
        reachable
            .iter()
            .copied()
            .map(|r1| {
                reachable
                    // The ones that are reachable now
                    .iter()
                    .copied()
                    .filter(move |r2| &r1 != r2)
                    .chain(
                        // The "almost reachable" ones
                        self.reachable_from(&r1, board)
                            .into_iter()
                            .filter(|r| !reachable.contains(&r)),
                    )
                    .map(move |r2| (r1, r2))
            })
            .flatten()
            .collect()
    }

    fn dice_actions(&self) -> impl Iterator<Item = Request> {
        (2..=12)
            .map(|i| Request::RollDice(Some(i)))
            .chain(std::iter::once(Request::RollDice(None)))
    }

    fn trade_actions(&self) -> impl Iterator<Item = Request> + '_ {
        // We push an empty Trade here since we don't actually want to enumerate
        // all the possible trades we could make with other players.  Instead,
        // we'll validate the values given to us after the fact.
        if self.hand.values().sum::<u8>() > 0
            && self.num_trades_offered_this_turn < MAX_TRADES_PER_TURN
        {
            Some(std::iter::once(Request::OfferTrade(Trade::empty())))
        } else {
            None
        }
        .into_iter()
        .flatten()
    }

    fn build_city_actions<'a>(&'a self, board: &'a Board) -> impl Iterator<Item = Request> + 'a {
        if self.can_afford(&boards::city_cost(board.board_type))
            && self.cities.len() < boards::max_cities(board.board_type)
        {
            Some(self.settlements.iter().map(|j| Request::BuildCity(*j)))
        } else {
            None
        }
        .into_iter()
        .flatten()
    }

    fn buy_dev_card_actions<'a>(&'a self, game: &'a Game) -> impl Iterator<Item = Request> + 'a {
        let board = &game.board;
        if self.can_afford(&boards::dev_card_cost(board.board_type)) && !game.dev_cards.is_empty() {
            Some(std::iter::once(Request::BuyDevCard))
        } else {
            None
        }
        .into_iter()
        .flatten()
    }

    fn build_road_actions(&self, board: &Board) -> impl Iterator<Item = Request> {
        if self.can_afford(&boards::road_cost(board.board_type))
            && self.roads.len() < boards::max_roads(board.board_type)
        {
            Some(
                self.reachable_roads(&board)
                    .into_iter()
                    .map(|r| Request::BuildRoad(r)),
            )
        } else {
            None
        }
        .into_iter()
        .flatten()
    }

    fn build_settlement_actions<'a>(
        &'a self,
        board: &'a Board,
    ) -> impl Iterator<Item = Request> + 'a {
        if self.can_afford(&boards::settlement_cost(board.board_type))
            && self.settlements.len() - self.cities.len()
                < boards::max_settlements(board.board_type)
        {
            Some(
                self.roads
                    .iter()
                    .map(move |r| board.junction_neighbors_of_road(r).into_iter())
                    .flatten()
                    .filter(|j| j.is_settleable)
                    .map(|j| j.index)
                    .map(|j| Request::BuildSettlement(j)),
            )
        } else {
            None
        }
        .into_iter()
        .flatten()
    }

    fn knight_actions(&self, board: &Board, robber_index: &Index) -> Vec<Request> {
        board
            .hexes()
            .into_iter()
            .map(|h| h.index)
            .filter(move |h| h != robber_index)
            .map(|h| Request::PlayKnight(h))
            .collect()
    }

    fn monopoly_actions(&self) -> Vec<Request> {
        Resource::all()
            .into_iter()
            .map(|r| Request::PlayMonopoly(r))
            .collect()
    }

    fn road_building_actions(&self, board: &Board) -> Vec<Request> {
        self.doubly_reachable_roads(&board)
            .into_iter()
            .map(|(r1, r2)| Request::PlayRoadBuilding((r1, r2)))
            .collect()
    }

    fn victory_point_actions(&self) -> Vec<Request> {
        vec![Request::PlayVictoryPoint]
    }

    fn year_of_plenty_actions(&self) -> Vec<Request> {
        Resource::all()
            .into_iter()
            .map(|r1| {
                Resource::all()
                    .into_iter()
                    .map(move |r2| Request::PlayYearOfPlenty((r1, r2)))
            })
            .flatten()
            .collect()
    }

    fn play_dev_card_actions<'a>(
        &'a self,
        board: &'a Board,
        has_rolled: bool,
        robber_index: &'a Index,
    ) -> impl Iterator<Item = Request> + 'a {
        if self.has_played_dev_card {
            None
        } else {
            Some(
                self.dev_cards
                    .iter()
                    .filter(move |dc| dc.state == DevCardState::CanPlay)
                    .map(|dc| dc.card)
                    .collect::<HashSet<_>>()
                    .into_iter()
                    .filter(move |dc| has_rolled || dc == &DevCard::VictoryPoint)
                    .map(move |dc| match dc {
                        // Sad that we need to ::collect() all of these into
                        // vectors, but I don't know of another way we could
                        // convince the compiler that these match arms are
                        // compatible ...
                        DevCard::Knight => self.knight_actions(board, robber_index),
                        DevCard::Monopoly => self.monopoly_actions(),
                        DevCard::RoadBuilding => self.road_building_actions(board),
                        DevCard::VictoryPoint => self.victory_point_actions(),
                        DevCard::YearOfPlenty => self.year_of_plenty_actions(),
                    })
                    .flatten(),
            )
        }
        .into_iter()
        .flatten()
    }

    pub fn available_actions(&self, game: &Game) -> Vec<Request> {
        if game.is_game_over().is_some() {
            return Vec::new();
        }

        let board = &game.board;

        match self.state {
            PlayerState::AfterBuildingFreeSettlement => self
                .reachable_roads(&board)
                .into_iter()
                .map(Request::BuildRoad)
                .collect(),

            PlayerState::AfterRollingSeven => {
                if self.num_to_discard > 0 {
                    // We push an empty vec here since we don't actually want to enumerate
                    // all the possible combinations of things we could discard. Instead,
                    // we'll validate the values given to us after the fact.
                    vec![Request::Discard(Vec::new())]
                } else if game.waiting_for_players_to_discard() {
                    // Do nothing -- just wait
                    Vec::new()
                } else {
                    board
                        .hexes()
                        .iter()
                        .filter(|h| &h.index != &game.robber_index)
                        .map(|h| Request::MoveRobber(h.index))
                        .collect()
                }
            }

            PlayerState::ChooseInitialResources => self
                .settlements
                .iter()
                .map(|j| Request::ChooseInitialResources(*j))
                .collect(),

            PlayerState::Root => {
                if game.is_first_round() || game.is_second_round() {
                    board
                        .junctions()
                        .iter()
                        .filter(|j| j.is_settleable)
                        .map(|j| Request::BuildSettlement(j.index))
                        .collect()
                } else if !game.has_rolled {
                    self.dice_actions()
                        .chain(self.play_dev_card_actions(
                            board,
                            game.has_rolled,
                            &game.robber_index,
                        ))
                        .collect()
                } else {
                    std::iter::once(Request::EndTurn)
                        .chain(self.trade_actions())
                        .chain(self.build_city_actions(board))
                        .chain(self.buy_dev_card_actions(game))
                        .chain(self.build_road_actions(board))
                        .chain(self.build_settlement_actions(board))
                        .chain(self.play_dev_card_actions(
                            board,
                            game.has_rolled,
                            &game.robber_index,
                        ))
                        .collect()
                }
            }

            PlayerState::Stealing => {
                board
                    .junction_neighbors_of_hex(&game.robber_index)
                    .into_iter()
                    .filter_map(|j| j.owner)
                    .filter(|owner| owner != &self.id)
                    .collect::<HashSet<_>>() // dedup
                    .into_iter()
                    .map(|owner| Request::StealFrom(owner))
                    .collect()
            }

            PlayerState::WaitForTurn => {
                if self.num_to_discard > 0 {
                    // We push an empty vec here since we don't actually want to enumerate
                    // all the possible combinations of things we could discard. Instead,
                    // we'll validate the values given to us after the fact.
                    return vec![Request::Discard(Vec::new())];
                }
                if let Some(Trade {
                    getting,
                    with: TradeWith::Players(players),
                    ..
                }) = &game.current_trade
                {
                    if !self.has_declined_trade
                        && self.can_afford(&getting)
                        && players.contains(&self.id)
                    {
                        return vec![Request::AcceptTrade, Request::DeclineTrade];
                    }
                }
                Vec::new() // can't do anything
            }

            PlayerState::WaitForTradeResponses => {
                if !game.can_anyone_accept_trade() {
                    // it's a 🐞!
                    eprintln!(
                        "Player({}): Waiting for trade responses, but no one can trade!",
                        self.id
                    );
                    panic!();
                }
                vec![Request::CancelTrade]
            }
        }
    }
}

#[cfg(test)]
mod test {
    use super::super::board::BoardType;
    use super::*;

    const ME: u64 = 100;
    const OTHER: u64 = 101;

    #[test]
    fn test_hand() {
        let mut p = Player::new(ME);
        assert!(p.can_afford(&vec![(Resource::Ore, 0)].into_iter().collect()));
        assert!(!p.can_afford(&vec![(Resource::Ore, 1)].into_iter().collect()));

        p.accrue(&vec![(Resource::Ore, 1)].into_iter().collect());
        assert!(p.can_afford(&vec![(Resource::Ore, 0)].into_iter().collect()));
        assert!(p.can_afford(&vec![(Resource::Ore, 1)].into_iter().collect()));
        assert!(!p.can_afford(&vec![(Resource::Ore, 2)].into_iter().collect()));
        assert!(!p.can_afford(
            &vec![(Resource::Ore, 1), (Resource::Wheat, 1)]
                .into_iter()
                .collect()
        ));

        p.spend(&vec![(Resource::Ore, 1)].into_iter().collect());
        assert!(p.can_afford(&vec![(Resource::Ore, 0)].into_iter().collect()));
        assert!(!p.can_afford(&vec![(Resource::Ore, 1)].into_iter().collect()));
    }

    #[test]
    fn test_bank_trade_rates() {
        let mut p = Player::new(ME);
        let i = Index::new(1, 1);

        let rates = Resource::all()
            .into_iter()
            .map(|resource| (resource, DEFAULT_BANK_TRADE_RATE))
            .collect();
        assert_eq!(p.bank_trade_rates, rates);

        p.push_settlement(&Junction::new(i, None));
        assert_eq!(p.bank_trade_rates, rates);

        p.push_settlement(&Junction::new(i, Some(Port::Any)));
        let rates = Resource::all()
            .into_iter()
            .map(|resource| (resource, GENERIC_PORT_BANK_TRADE_RATE))
            .collect();
        assert_eq!(p.bank_trade_rates, rates);

        p.push_settlement(&Junction::new(i, Some(Port::Only(Resource::Ore))));
        let rates = Resource::all()
            .into_iter()
            .map(|resource| {
                (
                    resource,
                    if resource == Resource::Ore {
                        SPECIALTY_PORT_BANK_TRADE_RATE
                    } else {
                        GENERIC_PORT_BANK_TRADE_RATE
                    },
                )
            })
            .collect();
        assert_eq!(p.bank_trade_rates, rates);
    }

    type Xy = (u16, u16);
    type XyPair = (Xy, Xy);

    fn doubly_reachable(
        me: &mut Player,
        my_roads: Vec<Xy>,
        other_settlements: Vec<Xy>,
    ) -> HashSet<XyPair> {
        let mut b = Board::new(BoardType::Standard, &mut rand::thread_rng());
        let my_roads = my_roads
            .into_iter()
            .map(|(x, y)| Index::new(x, y))
            .for_each(|r| {
                me.push_road(b.get_road(&r).unwrap());
                b.get_road_mut(&r).unwrap().owner = Some(ME);
            });
        other_settlements
            .into_iter()
            .map(|(x, y)| Index::new(x, y))
            .for_each(|j| b.get_junction_mut(&j).unwrap().owner = Some(OTHER));
        me.doubly_reachable_roads(&b)
            .into_iter()
            .map(|(r1, r2)| ((r1.x, r1.y), (r2.x, r2.y)))
            .collect()
    }

    #[test]
    fn test_doubly_reachable_roads() {
        let mut me = Player::new(ME);
        let pairs = doubly_reachable(&mut me, vec![], vec![]);
        assert_eq!(pairs, HashSet::new());

        //   456789012
        // 1     .   .
        // 2    a b c
        // 3   .   x
        // 4   d   e
        // 5   .   .
        // 6  f g / \
        //
        let a = (2, 7);
        let b = (2, 9);
        let c = (2, 11);
        let d = (4, 6);
        let e = (4, 10);
        let f = (6, 5);
        let g = (6, 7);
        let x = (3, 10);

        let pairs = doubly_reachable(&mut me, vec![a], vec![]);
        assert_eq!(
            pairs,
            vec![(b, c), (b, d), (b, e), (d, b), (d, f), (d, g)]
                .into_iter()
                .collect::<HashSet<XyPair>>()
        );

        let pairs = doubly_reachable(&mut me, vec![a], vec![x]);
        assert_eq!(
            pairs,
            vec![(b, d), (d, b), (d, f), (d, g)]
                .into_iter()
                .collect::<HashSet<XyPair>>()
        );
    }
}
