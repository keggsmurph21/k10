use rand::seq::SliceRandom;
use rand::Rng;

use serde::{Deserialize, Serialize};

use std::collections::HashMap;
use std::collections::HashSet;
use std::convert::TryFrom;

use super::action::{Error, Payload, Request, Response};
use super::board::{boards, Board, BoardType, Hex, HexType, Index, Junction};
use super::dev_card::DevCard;
use super::die::Die;
use super::player::{Player, PlayerId, PlayerState};
use super::resource::ResourceCounts;
use super::trade::{Trade, TradeWith};

type Players = HashMap<PlayerId, Player>;

const MIN_LARGEST_ARMY: u8 = 3;
const MIN_LONGEST_ROAD: u8 = 5;

#[derive(Debug, Deserialize, Serialize)]
pub struct Game {
    pub players: Players,
    player_order: Vec<PlayerId>,
    current_player_index: u8,
    victory_points_goal: u8,
    pub dev_cards: Vec<DevCard>,
    pub board: Board,
    pub robber_index: Index,
    turn_number: u32,
    pub has_rolled: bool,
    pub current_trade: Option<Trade>,
    has_largest_army: Option<PlayerId>,
    has_longest_road: Option<PlayerId>,
    #[serde(skip)]
    die: Die,
}

impl Game {
    pub fn new<R: Rng>(
        player_ids: Vec<PlayerId>,
        board_type: BoardType,
        victory_points_goal: u8,
        mut rng: &mut R,
    ) -> Option<Game> {
        match player_ids.as_slice() {
            [] => None,
            [_] => None,
            _ => Some({
                let mut player_order = player_ids.clone();
                player_order.shuffle(&mut rng);

                let board = Board::new(board_type, &mut rng);

                let mut game = Game {
                    players: player_ids
                        .into_iter()
                        .map(|player_id| (player_id, Player::new(player_id)))
                        .collect(),
                    player_order,
                    current_player_index: 0,
                    victory_points_goal,
                    dev_cards: boards::dev_cards(board_type, &mut rng),
                    robber_index: board
                        .hexes()
                        .iter()
                        .find(|hex| match hex {
                            Hex {
                                // The robber should be on the Desert to start :^)
                                hex_type: HexType::Desert,
                                ..
                            } => true,
                            _ => false,
                        })
                        .unwrap()
                        .index,
                    board,
                    turn_number: 0,
                    has_rolled: false,
                    current_trade: None,
                    has_largest_army: None,
                    has_longest_road: None,
                    die: Die::default(),
                };

                // Put the "current" player into the Root state
                let current_player = game.player_order[usize::from(game.current_player_index)];
                let current_player = game.players.get_mut(&u64::from(current_player)).unwrap();
                current_player.state = PlayerState::Root;

                game
            }),
        }
    }

    fn num_players(&self) -> u32 {
        u32::try_from(self.player_order.len()).unwrap()
    }

    fn round_number(&self) -> u32 {
        self.turn_number / self.num_players()
    }

    pub fn is_first_round(&self) -> bool {
        self.round_number() == 0
    }

    pub fn is_second_round(&self) -> bool {
        self.round_number() == 1
    }

    fn increment_turn(&mut self) {
        self.turn_number += 1;
        if self.turn_number == self.num_players() {
            // first round over, start snake-ing
        } else if self.turn_number == self.num_players() * 2 {
            // second round over, finish snake-ing
        } else if self.is_second_round() {
            // snake
            self.current_player_index -= 1;
        } else {
            // normal case
            self.current_player_index += 1;
            if u32::from(self.current_player_index) == self.num_players() {
                self.current_player_index = 0; // wrap
            }
        }

        self.has_rolled = false;
        self.players
            .values_mut()
            .for_each(Player::reset_for_new_turn);
        self.current_player_mut().state = PlayerState::Root;
    }

    pub fn current_player(&self) -> &Player {
        let player_id = self.player_order[usize::from(self.current_player_index)];
        self.players.get(&u64::from(player_id)).unwrap()
    }

    fn current_player_mut(&mut self) -> &mut Player {
        let player_id = self.player_order[usize::from(self.current_player_index)];
        self.players.get_mut(&u64::from(player_id)).unwrap()
    }

    #[must_use]
    pub fn is_game_over(&self) -> Option<PlayerId> {
        self.players
            .values()
            .filter(|p| p.victory_points.private >= self.victory_points_goal)
            .map(|p| p.id)
            .collect::<Vec<_>>()
            .pop()
    }

    #[must_use]
    fn build_road(&mut self, me: PlayerId, road: Index) -> Option<PlayerId> {
        // Returns whether playing this road made the game end!
        {
            let me = self.players.get_mut(&me).unwrap();
            let road = self.board.get_road_mut(&road).unwrap();

            road.owner = Some(me.id);
            me.push_road(&road);
        }

        if self.recalc_longest_road(me) {
            self.is_game_over()
        } else {
            None
        }
    }

    fn set_trade(&mut self, trade: Option<Trade>) {
        self.current_player_mut().state = if trade.is_none() {
            PlayerState::Root
        } else {
            PlayerState::WaitForTradeResponses
        };
        self.current_trade = trade;
        for player in self.players.values_mut() {
            player.has_declined_trade = false;
        }
    }

    #[must_use]
    fn move_robber(&mut self, me: PlayerId, hex: Index) -> bool {
        // Update the robber position and return whether there are any
        // players adjacent to the hex we just placed it on.
        self.robber_index = hex;
        for junction in self.board.junction_neighbors_of_hex(&hex) {
            if let Junction {
                owner: Some(owner), ..
            } = junction
            {
                if owner != &me {
                    return true;
                }
            }
        }
        return false;
    }

    fn can_anyone_afford(players: &Vec<&Player>, cost: &ResourceCounts) -> bool {
        players
            .iter()
            .fold(false, |acc, player| acc || player.can_afford(&cost))
    }

    pub fn waiting_for_players_to_discard(&self) -> bool {
        self.players
            .values()
            .fold(false, |acc, p| acc || p.num_to_discard > 0)
    }

    fn currently_trading_players(&self) -> Option<Vec<&Player>> {
        // At this point, we know that we've already validated all the
        // "with" player ids, so we can unwrap with confidence :^)
        match &self.current_trade {
            Some(Trade {
                with: TradeWith::Players(players),
                ..
            }) => Some(
                players
                    .iter()
                    .map(|player| self.players.get(&player).unwrap())
                    .collect(),
            ),
            _ => None,
        }
    }

    pub fn can_anyone_accept_trade(&self) -> bool {
        match self.currently_trading_players() {
            None => false,
            Some(players) => {
                let getting = &self.current_trade.as_ref().unwrap().getting;
                players.iter().fold(false, |acc, player| {
                    acc || (!player.has_declined_trade && player.can_afford(&getting))
                })
            }
        }
    }

    #[must_use]
    fn recalc_largest_army(&mut self, me: PlayerId) -> bool {
        let my_army_size = self.players.get(&me).unwrap().army_size;
        if my_army_size < MIN_LARGEST_ARMY {
            return false;
        }
        match self.has_largest_army {
            None => (),
            Some(other) if other == me => return false,
            Some(other) => {
                let other = self.players.get_mut(&other).unwrap();
                if other.army_size >= my_army_size {
                    return false;
                }
                other.victory_points.public -= 2;
                other.victory_points.private -= 2;
            }
        }
        let me = self.players.get_mut(&me).unwrap();
        me.victory_points.public += 2;
        me.victory_points.private += 2;
        self.has_largest_army = Some(me.id);
        true
    }

    #[must_use]
    fn recalc_longest_road(&mut self, me: PlayerId) -> bool {
        let my_longest_road = self.players.get(&me).unwrap().longest_road(&self.board);
        if my_longest_road < MIN_LONGEST_ROAD {
            return false;
        }
        match self.has_longest_road {
            None => (),
            Some(other) if other == me => return false,
            Some(other) => {
                let other = self.players.get_mut(&other).unwrap();
                if other.longest_road(&self.board) >= my_longest_road {
                    return false;
                }
                other.victory_points.public -= 2;
                other.victory_points.private -= 2;
            }
        }
        let me = self.players.get_mut(&me).unwrap();
        me.victory_points.public += 2;
        me.victory_points.private += 2;
        self.has_longest_road = Some(me.id);
        true
    }

    pub fn available_actions(&self, me: PlayerId) -> Option<Vec<Request>> {
        match self.players.get(&me) {
            None => None,
            Some(me) => Some(me.available_actions(&self)),
        }
    }

    pub fn make_move<R: Rng>(&mut self, me: PlayerId, req: Request, mut rng: &mut R) -> Response {
        let available_actions = match self.available_actions(me) {
            None => return Response::Err(Error::PlayerNotFound),
            Some(actions) => actions,
        };

        // NOTE: *Sometimes* an action that we can take won't actually
        //       be in this vector.  In particular, we don't enumerate
        //       all of the possible trades OR all of the possible discard
        //       combinations, so we need to handle those separately.
        let is_valid = if let Request::OfferTrade(_) = req {
            available_actions.contains(&Request::OfferTrade(Trade::empty()))
        } else if let Request::Discard(_) = req {
            available_actions.contains(&Request::Discard(vec![]))
        } else {
            available_actions.contains(&req)
        };

        if !is_valid {
            println!("Player {:?} cannot make move {:?}!", me, req);
            if available_actions.is_empty() {
                println!(" (no actions available)");
            } else {
                println!(" available actions:");
                for a in &available_actions {
                    println!(" * {:?}", a);
                }
            }
            return Response::Err(Error::InvalidAction);
        }

        match req {
            Request::AcceptTrade => {
                let (giving, getting) = {
                    let Trade {
                        giving, getting, ..
                    } = &self.current_trade.as_ref().unwrap();
                    (giving.clone(), getting.clone())
                };

                let me = self.players.get_mut(&me).unwrap();
                me.spend(&getting);
                me.accrue(&giving);

                let offerer = self.current_player_mut();
                offerer.spend(&giving);
                offerer.accrue(&getting);

                self.set_trade(None);
                Response::Ok(Payload::Null)
            }

            Request::BuildCity(junction) => {
                let board_type = self.board.board_type;
                let junction = self.board.get_junction_mut(&junction).unwrap();
                junction.has_city = true;

                let me = self.players.get_mut(&me).unwrap();
                me.spend(&boards::city_cost(board_type));
                me.push_city(&junction);
                me.victory_points.public += 1;
                me.victory_points.private += 1;
                me.state = PlayerState::Root;

                Response::Ok(Payload::Null)
            }

            Request::BuildRoad(road) => {
                let board_type = self.board.board_type;
                let is_first_round = self.is_first_round();
                let is_second_round = self.is_second_round();

                if let Some(winner) = self.build_road(me, road) {
                    return Response::Ok(Payload::GameOver(winner));
                }

                let me = self.players.get_mut(&me).unwrap();

                if !is_first_round && !is_second_round {
                    me.spend(&boards::road_cost(board_type));
                }

                me.state = {
                    if is_first_round {
                        PlayerState::WaitForTurn
                    } else if is_second_round {
                        PlayerState::ChooseInitialResources
                    } else {
                        PlayerState::Root
                    }
                };

                if is_first_round {
                    self.increment_turn();
                }

                Response::Ok(Payload::Null)
            }

            Request::BuildSettlement(junction) => {
                let board_type = self.board.board_type;
                let is_first_round = self.is_first_round();
                let is_second_round = self.is_second_round();

                {
                    let me = self.players.get_mut(&me).unwrap();

                    if !is_first_round && !is_second_round {
                        me.spend(&boards::settlement_cost(board_type));
                    }

                    {
                        let junction = self.board.get_junction_mut(&junction).unwrap();
                        junction.owner = Some(me.id);
                        junction.has_settlement = true;
                        junction.is_settleable = false;

                        me.push_settlement(&junction);
                        me.victory_points.public += 1;
                        me.victory_points.private += 1;
                        me.state = {
                            if is_first_round || is_second_round {
                                PlayerState::AfterBuildingFreeSettlement
                            } else {
                                PlayerState::Root
                            }
                        };
                    }
                }

                // Make sure we update the neighbors!
                let to_update = self
                    .board
                    .junction_neighbors_of_junction(&junction)
                    .into_iter()
                    .map(|j| j.index)
                    .collect::<HashSet<_>>();
                for neighbor in to_update {
                    let neighbor = self.board.get_junction_mut(&neighbor).unwrap();
                    neighbor.is_settleable = false;
                }

                // TODO: We don't have to make this calculation *unless* we
                //       settling on a junction bordered by two roads owned
                //       by the same person :^)
                if self.recalc_longest_road(me) {
                    if let Some(winner) = self.is_game_over() {
                        return Response::Ok(Payload::GameOver(winner));
                    }
                }

                Response::Ok(Payload::Null)
            }

            Request::BuyDevCard => {
                let board_type = self.board.board_type;
                let dev_card = self.dev_cards.pop().unwrap();

                {
                    let me = self.players.get_mut(&me).unwrap();
                    me.spend(&boards::dev_card_cost(board_type));
                    me.push_dev_card(dev_card);
                    me.state = PlayerState::Root;

                    // We want to immediately increment our *private* victory
                    // point total, but we don't want to broadcast it!
                    if &dev_card == &DevCard::VictoryPoint {
                        me.victory_points.private += 1;
                    }
                }

                if let Some(winner) = self.is_game_over() {
                    return Response::Ok(Payload::GameOver(winner));
                }

                Response::Ok(Payload::Drew(dev_card))
            }

            Request::CancelTrade => {
                self.set_trade(None);
                Response::Ok(Payload::Null)
            }

            Request::ChooseInitialResources(junction) => {
                let mut resources = ResourceCounts::new();
                for hex in self.board.hex_neighbors_of_junction(&junction) {
                    if let Hex {
                        hex_type: HexType::WithResource { resource: r, .. },
                        ..
                    } = hex
                    {
                        *resources.entry(*r).or_insert(0) += 1;
                    }
                }

                let me = self.players.get_mut(&me).unwrap();
                me.accrue(&resources);

                self.increment_turn();

                Response::Ok(Payload::Null)
            }

            Request::DeclineTrade => {
                let me = self.players.get_mut(&me).unwrap();
                me.has_declined_trade = true;
                if !self.can_anyone_accept_trade() {
                    // No one else to wait on!
                    self.set_trade(None);
                }
                Response::Ok(Payload::Null)
            }

            Request::Discard(resources) => {
                // NOTE: We need to validate our input here!
                let me = self.players.get_mut(&me).unwrap();
                let mut diff = ResourceCounts::new();
                let mut sum = 0;
                for resource in resources {
                    *diff.entry(resource).or_insert(0) += 1;
                    sum += 1;
                }
                if !me.can_afford(&diff) {
                    return Response::Err(Error::CannotAfford);
                }
                if sum > me.num_to_discard {
                    return Response::Err(Error::DiscardingTooMany);
                }
                me.spend(&diff);
                me.num_to_discard -= sum;
                Response::Ok(Payload::Null)
            }

            Request::EndTurn => {
                let me = self.players.get_mut(&me).unwrap();
                me.state = PlayerState::WaitForTurn;
                self.increment_turn();
                Response::Ok(Payload::Null)
            }

            Request::MoveRobber(hex) => {
                let can_steal = self.move_robber(me, hex);
                let me = self.players.get_mut(&me).unwrap();
                me.state = if can_steal {
                    PlayerState::Stealing
                } else {
                    PlayerState::Root
                };
                Response::Ok(Payload::Null)
            }

            Request::OfferTrade(trade) => {
                // NOTE: We need to validate our input here!

                let Trade {
                    giving,
                    getting,
                    with,
                } = &trade;

                if !self.players.get(&me).unwrap().can_afford(&giving) {
                    return Response::Err(Error::CannotAfford);
                }

                if giving == getting {
                    return Response::Err(Error::InvalidTrade);
                }

                if let TradeWith::Players(players) = with {
                    match players
                        .iter()
                        .map(|player| {
                            self.players
                                .get(&player)
                                .map(|p| if p.id == me { None } else { Some(p) })
                        })
                        .collect::<Option<Option<Vec<_>>>>()
                    {
                        None => return Response::Err(Error::PlayerNotFound),
                        Some(None) => return Response::Err(Error::InvalidTrade), // trading with self
                        Some(Some(players)) => {
                            if Game::can_anyone_afford(&players, &getting) {
                                self.set_trade(Some(trade));
                            }
                        }
                    }
                } else {
                    let me = self.players.get_mut(&me).unwrap();
                    let mut purchasing_power = 0;
                    for (resource, count) in giving {
                        let rate = me.bank_trade_rate(&resource);
                        if count % rate != 0 {
                            println!(
                                "Cannot offer {} {:?}: not a multiple of {}",
                                count, resource, rate
                            );
                            return Response::Err(Error::InvalidTrade);
                        }
                        purchasing_power += 1;
                    }
                    let num_getting = getting.values().sum::<u8>();
                    if num_getting != purchasing_power {
                        println!(
                            "Requested {} resource, but only offered enough for {}",
                            num_getting, purchasing_power
                        );
                        return Response::Err(Error::InvalidTrade);
                    }

                    // Ok! Do it immediately
                    me.spend(&giving);
                    me.accrue(&getting);
                }

                Response::Ok(Payload::Null)
            }

            Request::PlayKnight(hex) => {
                let can_steal = self.move_robber(me, hex);

                {
                    let me = self.players.get_mut(&me).unwrap();
                    me.army_size += 1;
                    me.state = if can_steal {
                        PlayerState::Stealing
                    } else {
                        PlayerState::Root
                    };
                    me.use_dev_card(&DevCard::Knight);
                }

                if self.recalc_largest_army(me) {
                    if let Some(winner) = self.is_game_over() {
                        return Response::Ok(Payload::GameOver(winner));
                    }
                }

                Response::Ok(Payload::Null)
            }

            Request::PlayMonopoly(resource) => {
                let mut sum = 0;
                let mut diff = ResourceCounts::new();
                for player in self.players.values_mut() {
                    if player.id == me {
                        continue;
                    }
                    if let Some(count) = player.hand.get(&resource) {
                        sum += count;
                        *diff.entry(resource).or_insert(0) = *count;
                        player.spend(&diff);
                    }
                }
                let me = self.players.get_mut(&me).unwrap();
                *diff.entry(resource).or_insert(0) = sum;
                me.accrue(&diff);
                me.use_dev_card(&DevCard::Monopoly);
                Response::Ok(Payload::Monopolized(sum))
            }

            Request::PlayRoadBuilding(roads) => {
                let (r1, r2) = roads;
                if let Some(winner) = self.build_road(me, r1) {
                    return Response::Ok(Payload::GameOver(winner));
                }
                if let Some(winner) = self.build_road(me, r2) {
                    return Response::Ok(Payload::GameOver(winner));
                }
                let me = self.players.get_mut(&me).unwrap();
                me.use_dev_card(&DevCard::RoadBuilding);
                Response::Ok(Payload::Null)
            }

            Request::PlayVictoryPoint => {
                let me = self.players.get_mut(&me).unwrap();
                // We already incremented *private* points when we drew
                // the card :^)
                me.victory_points.public += 1;
                me.use_dev_card(&DevCard::VictoryPoint);
                Response::Ok(Payload::Null)
            }

            Request::PlayYearOfPlenty(resources) => {
                let (r1, r2) = resources;
                let mut resources = ResourceCounts::new();
                *resources.entry(r1).or_insert(0) += 1;
                *resources.entry(r2).or_insert(0) += 1;
                let me = self.players.get_mut(&me).unwrap();
                me.accrue(&resources);
                me.use_dev_card(&DevCard::YearOfPlenty);
                Response::Ok(Payload::Null)
            }

            Request::RollDice(roll) => {
                let roll = match roll {
                    Some(roll) => roll,
                    None => {
                        let should_allow_sevens = self.round_number() > 4;
                        loop {
                            let r1 = self.die.roll(&mut rng);
                            let r2 = self.die.roll(&mut rng);
                            let roll = r1 + r2;
                            if roll != 7 || should_allow_sevens {
                                break roll;
                            }
                        }
                    }
                };
                self.has_rolled = true;
                if roll == 7 {
                    for player in self.players.values_mut() {
                        let purse_size = player.hand.values().sum::<u8>();
                        if purse_size > 7 {
                            player.num_to_discard = purse_size / 2;
                        }
                    }
                    let me = self.players.get_mut(&me).unwrap();
                    me.state = PlayerState::AfterRollingSeven;
                } else {
                    for hex in self.board.hexes() {
                        if let Hex {
                            index,
                            hex_type:
                                HexType::WithResource {
                                    resource,
                                    roll: hex_roll,
                                },
                        } = hex
                        {
                            if &roll != hex_roll {
                                continue;
                            }
                            if index == &self.robber_index {
                                continue; // tough
                            }
                            for junction in self.board.junction_neighbors_of_hex(&index) {
                                if let Junction {
                                    owner: Some(owner),
                                    has_city,
                                    ..
                                } = junction
                                {
                                    let owner = self.players.get_mut(owner).unwrap();
                                    owner.accrue(
                                        &vec![(*resource, if *has_city { 2 } else { 1 })]
                                            .into_iter()
                                            .collect(),
                                    );
                                }
                            }
                        }
                    }
                }
                Response::Ok(Payload::Rolled(roll))
            }

            Request::StealFrom(from) => {
                let mut diff = ResourceCounts::new();

                let stolen = {
                    let from = self.players.get_mut(&from).unwrap();
                    if from.hand.values().sum::<u8>() == 0 {
                        None // Got em
                    } else {
                        let stolen = {
                            let mut cards = Vec::new();
                            for (resource, count) in &from.hand {
                                for _ in 0..*count {
                                    cards.push(resource);
                                }
                            }
                            **cards.choose(&mut rng).unwrap()
                        };
                        *diff.entry(stolen).or_insert(0) = 1;
                        from.spend(&diff);
                        Some(stolen)
                    }
                };

                let me = self.players.get_mut(&me).unwrap();
                me.accrue(&diff);
                me.state = PlayerState::Root;

                Response::Ok(Payload::Stole(stolen))
            }
        }
    }
}
