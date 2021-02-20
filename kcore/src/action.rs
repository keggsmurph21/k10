use serde::{Deserialize, Serialize};

use super::board::Index;
use super::dev_card::DevCard;
use super::player::PlayerId;
use super::resource::Resource;
use super::trade::Trade;

type HexIndex = Index;
type JunctionIndex = Index;
type RoadIndex = Index;

#[derive(Debug, Deserialize, PartialEq, Serialize)]
pub enum Request {
    AcceptTrade,
    BuildCity(JunctionIndex),
    BuildRoad(RoadIndex),
    BuildSettlement(JunctionIndex),
    BuyDevCard,
    CancelTrade,
    ChooseInitialResources(JunctionIndex),
    DeclineTrade,
    Discard(Vec<Resource>),
    EndTurn,
    MoveRobber(HexIndex),
    OfferTrade(Trade),
    PlayKnight(HexIndex),
    PlayMonopoly(Resource),
    PlayRoadBuilding((RoadIndex, RoadIndex)),
    PlayVictoryPoint,
    PlayYearOfPlenty((Resource, Resource)),
    RollDice(Option<u8>),
    StealFrom(PlayerId),
}

#[derive(Debug, Deserialize, Serialize)]
pub enum Error {
    CannotAfford,
    DiscardingTooMany,
    InvalidAction,
    InvalidTrade,
    PlayerNotFound,
}

#[derive(Debug, Deserialize, Serialize)]
pub enum Payload {
    Drew(DevCard),
    GameOver(u64),
    Monopolized(u8),
    Null,
    Rolled(u8),
    Stole(Option<Resource>),
}

pub type Response = Result<Payload, Error>;
