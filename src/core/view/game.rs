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

pub mod test {

    // TODO: Share this with k10::core & friends
    pub type Result<T> = std::result::Result<T, std::io::Error>;

    use std::collections::HashSet;

    use super::super::super::action;
    use super::super::super::board::Hex;
    use super::super::super::board::HexResource;
    use super::super::super::board::HexType;
    use super::super::super::board::Junction;
    use super::super::super::board::Node;
    use super::super::super::board::Port;
    use super::super::super::board::Road;
    use super::super::super::player::PlayerState;
    use super::super::super::player::VictoryPoints;
    use super::super::super::resource::Resource;
    use super::super::super::resource::ResourceCounts;
    use super::super::super::BoardType;
    use super::super::super::Player;
    use super::*;

    struct Renderer {}

    enum Item {
        Empty,
        Hex(HexType),
        Robber(HexType),
        Road(RoadOrientation, Option<PlayerId>, Option<Port>),
        Junction(Junction),
    }

    enum ColorType {
        None,
        Ocean,
        Desert,
        Resource(Resource),
        Player(Option<PlayerId>),
    }

    enum CellValue {
        None,
        Ocean,
        Desert,
        Int(u8),
        Robber(u8),
        Road(RoadOrientation),
        City,
        Settlement,
        Unsettled,
    }

    struct Cell {
        color: ColorType,
        value: CellValue,
    }

    #[derive(Clone, Debug)]
    enum RoadOrientation {
        TwelveSix, // |
        TwoEight,  // /
        FourTen,   // \
    }

    impl Renderer {
        pub fn new() -> Self {
            Self {}
        }

        pub fn render(&self, view: &GameView) -> Result<()> {
            eprintln!("{}", view.board.nodes.len());
            eprintln!("{}", view.board.nodes[0].len());

            let logical_grid: Vec<Vec<Item>> = view
                .board
                .nodes
                .iter()
                .map(|row| {
                    row.iter()
                        .map(|cell| match cell {
                            None => Item::Empty,
                            Some(Node::Hex(Hex { hex_type, index })) => {
                                if index == &view.robber_index {
                                    Item::Robber(hex_type.clone())
                                } else {
                                    Item::Hex(hex_type.clone())
                                }
                            }
                            Some(Node::Road(Road {
                                index: Index { x, y },
                                owner,
                            })) => {
                                // TODO: `x` and `y` are backwards ....
                                let x = *x as usize;
                                let y = *y as usize;

                                let nw = &view.board.nodes[x - 1][y - 1];
                                let n = &view.board.nodes[x - 1][y];
                                let ne = &view.board.nodes[x - 1][y + 1];

                                let sw = &view.board.nodes[x + 1][y - 1];
                                let s = &view.board.nodes[x + 1][y];
                                let se = &view.board.nodes[x + 1][y + 1];

                                let mut foo = None;

                                for (orientation, pair) in &[
                                    (RoadOrientation::FourTen, (nw, se)),
                                    (RoadOrientation::TwelveSix, (n, s)),
                                    (RoadOrientation::TwoEight, (ne, sw)),
                                ] {
                                    match pair {
                                        (
                                            Some(Node::Junction(upper)),
                                            Some(Node::Junction(lower)),
                                        ) => {
                                            let port = if upper.port.is_some()
                                                && upper.port == lower.port
                                            {
                                                upper.port.clone()
                                            } else {
                                                None
                                            };
                                            foo = Some((orientation.clone(), port));
                                            break;
                                        }
                                        _ => {}
                                    }
                                }

                                let (orientation, port) = foo.unwrap();

                                Item::Road(orientation, owner.clone(), port.clone())
                            }
                            Some(Node::Junction(j)) => Item::Junction(j.clone()),
                        })
                        .collect()
                })
                .collect();

            for row in logical_grid {
                for cell in row {
                    let ch = match cell {
                        Item::Empty => Cell {
                            color: ColorType::None,
                            value: CellValue::None,
                        },
                        Item::Robber(HexType::WithResource(HexResource { resource, roll })) => {
                            Cell {
                                color: ColorType::Resource(resource),
                                value: CellValue::Robber(roll),
                            }
                        }
                        Item::Robber(HexType::Desert) => Cell {
                            color: ColorType::Desert,
                            value: CellValue::Robber(0),
                        },
                        Item::Robber(_) => panic!(), // Can't have robber on ocean
                        Item::Hex(HexType::Desert) => Cell {
                            color: ColorType::Desert,
                            value: CellValue::Desert,
                        },
                        Item::Hex(HexType::Ocean) => Cell {
                            color: ColorType::Ocean,
                            value: CellValue::Ocean,
                        },
                        Item::Hex(HexType::WithResource(HexResource { resource, roll })) => Cell {
                            color: ColorType::Resource(resource),
                            value: CellValue::Int(roll),
                        },
                        Item::Road(orientation, owner, _port) => Cell {
                            color: ColorType::Player(owner),
                            value: CellValue::Road(orientation),
                        },
                        Item::Junction(Junction {
                            has_settlement,
                            has_city,
                            owner,
                            ..
                        }) => {
                            let value = if has_city {
                                CellValue::City
                            } else if has_settlement {
                                CellValue::Settlement
                            } else {
                                CellValue::Unsettled
                            };
                            Cell {
                                color: ColorType::Player(owner),
                                value,
                            }
                        }
                    };

                    let escape = match ch {
                        Cell {
                            color: ColorType::None,
                            ..
                        } => "\x1b[0m",
                        Cell {
                            color: ColorType::Ocean,
                            ..
                        } => "\x1b[38;5;18m",
                        Cell {
                            color: ColorType::Desert,
                            ..
                        } => "\x1b[38;5;231m",
                        Cell {
                            color: ColorType::Resource(Resource::Brick),
                            ..
                        } => "\x1b[38;5;166;1m",
                        Cell {
                            color: ColorType::Resource(Resource::Ore),
                            ..
                        } => "\x1b[38;5;248;1m",
                        Cell {
                            color: ColorType::Resource(Resource::Sheep),
                            ..
                        } => "\x1b[38;5;46;1m",
                        Cell {
                            color: ColorType::Resource(Resource::Wheat),
                            ..
                        } => "\x1b[38;5;230;1m",
                        Cell {
                            color: ColorType::Resource(Resource::Wood),
                            ..
                        } => "\x1b[38;5;22;1m",
                        Cell {
                            color: ColorType::Player(None),
                            ..
                        } => "\x1b[0m",
                        Cell {
                            color: ColorType::Player(Some(_)),
                            ..
                        } => "\x1b[38;5;9m", // TODO
                    };

                    let value = match ch {
                        Cell {
                            value: CellValue::None,
                            ..
                        } => "  ",
                        Cell {
                            value: CellValue::Ocean,
                            ..
                        } => "~~",
                        Cell {
                            value: CellValue::Desert,
                            ..
                        } => "--",
                        Cell {
                            value: CellValue::Int(2),
                            ..
                        } => " 2",
                        Cell {
                            value: CellValue::Int(3),
                            ..
                        } => " 3",
                        Cell {
                            value: CellValue::Int(4),
                            ..
                        } => " 4",
                        Cell {
                            value: CellValue::Int(5),
                            ..
                        } => " 5",
                        Cell {
                            value: CellValue::Int(6),
                            ..
                        } => " 6",
                        Cell {
                            value: CellValue::Int(8),
                            ..
                        } => " 8",
                        Cell {
                            value: CellValue::Int(9),
                            ..
                        } => " 9",
                        Cell {
                            value: CellValue::Int(10),
                            ..
                        } => "10",
                        Cell {
                            value: CellValue::Int(11),
                            ..
                        } => "11",
                        Cell {
                            value: CellValue::Int(12),
                            ..
                        } => "12",
                        Cell {
                            value: CellValue::Int(_),
                            ..
                        } => panic!(),
                        Cell {
                            value: CellValue::Robber(0),
                            ..
                        } => "XX",
                        Cell {
                            value: CellValue::Robber(2),
                            ..
                        } => " 2",
                        Cell {
                            value: CellValue::Robber(3),
                            ..
                        } => " 3",
                        Cell {
                            value: CellValue::Robber(4),
                            ..
                        } => " 4",
                        Cell {
                            value: CellValue::Robber(5),
                            ..
                        } => " 5",
                        Cell {
                            value: CellValue::Robber(6),
                            ..
                        } => " 6",
                        Cell {
                            value: CellValue::Robber(8),
                            ..
                        } => " 8",
                        Cell {
                            value: CellValue::Robber(9),
                            ..
                        } => " 9",
                        Cell {
                            value: CellValue::Robber(10),
                            ..
                        } => "10",
                        Cell {
                            value: CellValue::Robber(11),
                            ..
                        } => "11",
                        Cell {
                            value: CellValue::Robber(12),
                            ..
                        } => "12",
                        Cell {
                            value: CellValue::Robber(_),
                            ..
                        } => panic!(),
                        Cell {
                            value: CellValue::Road(RoadOrientation::TwelveSix),
                            ..
                        } => " |",
                        Cell {
                            value: CellValue::Road(RoadOrientation::TwoEight),
                            ..
                        } => " /",
                        Cell {
                            value: CellValue::Road(RoadOrientation::FourTen),
                            ..
                        } => " \\",
                        Cell {
                            value: CellValue::City,
                            ..
                        } => " C",
                        Cell {
                            value: CellValue::Settlement,
                            ..
                        } => " s",
                        Cell {
                            value: CellValue::Unsettled,
                            ..
                        } => " .",
                    };

                    eprint!("{}{}\x1b[0m", escape, value);
                }
                eprint!("\n");
            }

            Ok(())
        }
    }

    pub fn test_view() -> Result<()> {
        let view = GameView {
            players: vec![
                PlayerView::Me(MeView {
                    player: Player {
                        id: 2,
                        state: PlayerState::Root,
                        hand: ResourceCounts::new(),
                        victory_points: VictoryPoints {
                            public: 0,
                            private: 0,
                        },
                        cities: HashSet::new(),
                        roads: HashSet::new(),
                        settlements: HashSet::new(),
                        army_size: 0,
                        dev_cards: vec![],
                        has_played_dev_card: false,
                        num_trades_offered_this_turn: 0,
                        num_to_discard: 0,
                        has_declined_trade: false,
                        bank_trade_rates: vec![
                            (Resource::Ore, 4),
                            (Resource::Wood, 4),
                            (Resource::Wheat, 4),
                            (Resource::Brick, 4),
                            (Resource::Sheep, 4),
                        ]
                        .into_iter()
                        .collect(),
                    },
                    actions: vec![
                        action::Request::BuildSettlement(Index { x: 1, y: 8 }),
                        action::Request::BuildSettlement(Index { x: 1, y: 12 }),
                        action::Request::BuildSettlement(Index { x: 1, y: 16 }),
                        action::Request::BuildSettlement(Index { x: 3, y: 6 }),
                        action::Request::BuildSettlement(Index { x: 3, y: 10 }),
                        action::Request::BuildSettlement(Index { x: 3, y: 14 }),
                        action::Request::BuildSettlement(Index { x: 3, y: 18 }),
                        action::Request::BuildSettlement(Index { x: 5, y: 6 }),
                        action::Request::BuildSettlement(Index { x: 5, y: 10 }),
                        action::Request::BuildSettlement(Index { x: 5, y: 14 }),
                        action::Request::BuildSettlement(Index { x: 5, y: 18 }),
                        action::Request::BuildSettlement(Index { x: 7, y: 4 }),
                        action::Request::BuildSettlement(Index { x: 7, y: 8 }),
                        action::Request::BuildSettlement(Index { x: 7, y: 12 }),
                        action::Request::BuildSettlement(Index { x: 7, y: 16 }),
                        action::Request::BuildSettlement(Index { x: 7, y: 20 }),
                        action::Request::BuildSettlement(Index { x: 9, y: 4 }),
                        action::Request::BuildSettlement(Index { x: 9, y: 8 }),
                        action::Request::BuildSettlement(Index { x: 9, y: 12 }),
                        action::Request::BuildSettlement(Index { x: 9, y: 16 }),
                        action::Request::BuildSettlement(Index { x: 9, y: 20 }),
                        action::Request::BuildSettlement(Index { x: 11, y: 2 }),
                        action::Request::BuildSettlement(Index { x: 11, y: 6 }),
                        action::Request::BuildSettlement(Index { x: 11, y: 10 }),
                        action::Request::BuildSettlement(Index { x: 11, y: 14 }),
                        action::Request::BuildSettlement(Index { x: 11, y: 18 }),
                        action::Request::BuildSettlement(Index { x: 11, y: 22 }),
                        action::Request::BuildSettlement(Index { x: 13, y: 2 }),
                        action::Request::BuildSettlement(Index { x: 13, y: 6 }),
                        action::Request::BuildSettlement(Index { x: 13, y: 10 }),
                        action::Request::BuildSettlement(Index { x: 13, y: 14 }),
                        action::Request::BuildSettlement(Index { x: 13, y: 18 }),
                        action::Request::BuildSettlement(Index { x: 13, y: 22 }),
                        action::Request::BuildSettlement(Index { x: 15, y: 4 }),
                        action::Request::BuildSettlement(Index { x: 15, y: 8 }),
                        action::Request::BuildSettlement(Index { x: 15, y: 12 }),
                        action::Request::BuildSettlement(Index { x: 15, y: 16 }),
                        action::Request::BuildSettlement(Index { x: 15, y: 20 }),
                        action::Request::BuildSettlement(Index { x: 17, y: 4 }),
                        action::Request::BuildSettlement(Index { x: 17, y: 8 }),
                        action::Request::BuildSettlement(Index { x: 17, y: 12 }),
                        action::Request::BuildSettlement(Index { x: 17, y: 16 }),
                        action::Request::BuildSettlement(Index { x: 17, y: 20 }),
                        action::Request::BuildSettlement(Index { x: 19, y: 6 }),
                        action::Request::BuildSettlement(Index { x: 19, y: 10 }),
                        action::Request::BuildSettlement(Index { x: 19, y: 14 }),
                        action::Request::BuildSettlement(Index { x: 19, y: 18 }),
                        action::Request::BuildSettlement(Index { x: 21, y: 6 }),
                        action::Request::BuildSettlement(Index { x: 21, y: 10 }),
                        action::Request::BuildSettlement(Index { x: 21, y: 14 }),
                        action::Request::BuildSettlement(Index { x: 21, y: 18 }),
                        action::Request::BuildSettlement(Index { x: 23, y: 8 }),
                        action::Request::BuildSettlement(Index { x: 23, y: 12 }),
                        action::Request::BuildSettlement(Index { x: 23, y: 16 }),
                    ],
                }),
                PlayerView::Other(OtherView {
                    id: 1,
                    num_resources: 0,
                    public_victory_points: 0,
                    cities: HashSet::new(),
                    roads: HashSet::new(),
                    settlements: HashSet::new(),
                    army_size: 0,
                    num_dev_cards: 0,
                    num_trades_offered_this_turn: 0,
                    num_to_discard: 0,
                    has_declined_trade: false,
                }),
            ],
            current_player_index: 0,
            victory_points_goal: 8,
            num_dev_cards: 25,
            board: Board {
                board_type: BoardType::Standard,
                nodes: vec![
                    vec![
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 0, y: 6 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 0, y: 10 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 0, y: 14 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 0, y: 18 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 1, y: 8 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Only(Resource::Sheep)),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 1, y: 12 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Only(Resource::Ore)),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 1, y: 16 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 2, y: 7 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 2, y: 9 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 2, y: 11 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 2, y: 13 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 2, y: 15 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 2, y: 17 },
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 3, y: 6 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Only(Resource::Sheep)),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 3, y: 10 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 3, y: 14 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Only(Resource::Ore)),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 3, y: 18 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 4, y: 4 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 4, y: 6 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 4, y: 8 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Sheep,
                                roll: 5,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 4, y: 10 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 4, y: 12 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Ore,
                                roll: 6,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 4, y: 14 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 4, y: 16 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Brick,
                                roll: 8,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 4, y: 18 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 4, y: 20 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 5, y: 6 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 5, y: 10 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 5, y: 14 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 5, y: 18 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Any),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 6, y: 5 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 6, y: 7 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 6, y: 9 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 6, y: 11 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 6, y: 13 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 6, y: 15 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 6, y: 17 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 6, y: 19 },
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 7, y: 4 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Only(Resource::Wood)),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 7, y: 8 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 7, y: 12 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 7, y: 16 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 7, y: 20 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Any),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 8, y: 2 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 8, y: 4 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 8, y: 6 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Wheat,
                                roll: 11,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 8, y: 8 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 8, y: 10 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Sheep,
                                roll: 10,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 8, y: 12 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 8, y: 14 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Sheep,
                                roll: 9,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 8, y: 16 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 8, y: 18 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Wheat,
                                roll: 6,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 8, y: 20 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 8, y: 22 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 9, y: 4 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Only(Resource::Wood)),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 9, y: 8 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 9, y: 12 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 9, y: 16 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 9, y: 20 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 10, y: 3 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 10, y: 5 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 10, y: 7 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 10, y: 9 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 10, y: 11 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 10, y: 13 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 10, y: 15 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 10, y: 17 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 10, y: 19 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 10, y: 21 },
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 11, y: 2 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 11, y: 6 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 11, y: 10 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 11, y: 14 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 11, y: 18 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 11, y: 22 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Any),
                            owner: None,
                        })),
                        None,
                        None,
                    ],
                    vec![
                        Some(Node::Hex(Hex {
                            index: Index { x: 12, y: 0 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 12, y: 2 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 12, y: 4 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Ore,
                                roll: 5,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 12, y: 6 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 12, y: 8 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Sheep,
                                roll: 2,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 12, y: 10 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 12, y: 12 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Brick,
                                roll: 3,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 12, y: 14 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 12, y: 16 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Wood,
                                roll: 9,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 12, y: 18 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 12, y: 20 },
                            hex_type: HexType::Desert,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 12, y: 22 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 12, y: 24 },
                            hex_type: HexType::Ocean,
                        })),
                    ],
                    vec![
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 13, y: 2 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 13, y: 6 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 13, y: 10 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 13, y: 14 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 13, y: 18 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 13, y: 22 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Any),
                            owner: None,
                        })),
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 14, y: 3 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 14, y: 5 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 14, y: 7 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 14, y: 9 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 14, y: 11 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 14, y: 13 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 14, y: 15 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 14, y: 17 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 14, y: 19 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 14, y: 21 },
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 15, y: 4 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Only(Resource::Wheat)),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 15, y: 8 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 15, y: 12 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 15, y: 16 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 15, y: 20 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 16, y: 2 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 16, y: 4 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 16, y: 6 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Ore,
                                roll: 4,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 16, y: 8 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 16, y: 10 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Brick,
                                roll: 10,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 16, y: 12 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 16, y: 14 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Wheat,
                                roll: 8,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 16, y: 16 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 16, y: 18 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Wheat,
                                roll: 3,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 16, y: 20 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 16, y: 22 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 17, y: 4 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Only(Resource::Wheat)),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 17, y: 8 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 17, y: 12 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 17, y: 16 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 17, y: 20 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Any),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 18, y: 5 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 18, y: 7 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 18, y: 9 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 18, y: 11 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 18, y: 13 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 18, y: 15 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 18, y: 17 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 18, y: 19 },
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 19, y: 6 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 19, y: 10 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 19, y: 14 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 19, y: 18 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Any),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 20, y: 4 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 20, y: 6 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 20, y: 8 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Wood,
                                roll: 11,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 20, y: 10 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 20, y: 12 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Wood,
                                roll: 4,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 20, y: 14 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 20, y: 16 },
                            hex_type: HexType::WithResource(HexResource {
                                resource: Resource::Wood,
                                roll: 12,
                            }),
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 20, y: 18 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 20, y: 20 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 21, y: 6 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Only(Resource::Brick)),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 21, y: 10 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 21, y: 14 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Any),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 21, y: 18 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 22, y: 7 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 22, y: 9 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 22, y: 11 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 22, y: 13 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 22, y: 15 },
                            owner: None,
                        })),
                        None,
                        Some(Node::Road(Road {
                            index: Index { x: 22, y: 17 },
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 23, y: 8 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Only(Resource::Brick)),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 23, y: 12 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: Some(Port::Any),
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Junction(Junction {
                            index: Index { x: 23, y: 16 },
                            has_settlement: false,
                            has_city: false,
                            is_settleable: true,
                            port: None,
                            owner: None,
                        })),
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                    ],
                    vec![
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 24, y: 6 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 24, y: 10 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 24, y: 14 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        None,
                        None,
                        Some(Node::Hex(Hex {
                            index: Index { x: 24, y: 18 },
                            hex_type: HexType::Ocean,
                        })),
                        None,
                        None,
                        None,
                        None,
                        None,
                        None,
                    ],
                ],
            },
            robber_index: Index { x: 12, y: 20 },
            turn_number: 0,
            has_rolled: false,
            current_trade: None,
            has_largest_army: None,
            has_longest_road: None,
        };

        let renderer = Renderer::new();
        renderer.render(&view)?;

        Ok(())
    }
}
