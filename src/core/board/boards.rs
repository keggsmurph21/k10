// All pre-defined board definitions will live here.  We'll just
// do a dispatch for all of the static data :/

use rand::seq::SliceRandom;
use rand::Rng;

use super::super::dev_card::DevCard;
use super::*;

pub fn city_cost(board_type: BoardType) -> ResourceCounts {
    match board_type {
        BoardType::Standard => vec![(Resource::Ore, 3), (Resource::Wheat, 2)]
            .into_iter()
            .collect(),
    }
}

pub fn dev_card_cost(board_type: BoardType) -> ResourceCounts {
    match board_type {
        BoardType::Standard => vec![
            (Resource::Ore, 1),
            (Resource::Sheep, 1),
            (Resource::Wheat, 1),
        ]
        .into_iter()
        .collect(),
    }
}

pub fn road_cost(board_type: BoardType) -> ResourceCounts {
    match board_type {
        BoardType::Standard => vec![(Resource::Brick, 1), (Resource::Wood, 1)]
            .into_iter()
            .collect(),
    }
}

pub fn settlement_cost(board_type: BoardType) -> ResourceCounts {
    match board_type {
        BoardType::Standard => vec![
            (Resource::Brick, 1),
            (Resource::Sheep, 1),
            (Resource::Wheat, 1),
            (Resource::Wood, 1),
        ]
        .into_iter()
        .collect(),
    }
}

pub fn dev_cards<R: Rng>(board_type: BoardType, mut rng: &mut R) -> Vec<DevCard> {
    match board_type {
        BoardType::Standard => standard::dev_cards(&mut rng),
    }
}

pub fn max_cities(board_type: BoardType) -> usize {
    match board_type {
        BoardType::Standard => 4,
    }
}

pub fn max_roads(board_type: BoardType) -> usize {
    match board_type {
        BoardType::Standard => 15,
    }
}

pub fn max_settlements(board_type: BoardType) -> usize {
    // Not incl. cities
    match board_type {
        BoardType::Standard => 5,
    }
}

pub mod standard {
    // For stuff that's too big to be inline :^)

    use super::*;

    pub fn nodes<R: Rng>(mut rng: &mut R) -> Nodes {
        let width: u16 = 25;
        let height: u16 = 25;

        let mut nodes = (0..height)
            .map(|_| (0..width).map(|_| None).collect())
            .collect::<Nodes>();

        let mut hex_resource_queue = vec![
            None,
            Some(Resource::Brick),
            Some(Resource::Brick),
            Some(Resource::Brick),
            Some(Resource::Ore),
            Some(Resource::Ore),
            Some(Resource::Ore),
            Some(Resource::Sheep),
            Some(Resource::Sheep),
            Some(Resource::Sheep),
            Some(Resource::Sheep),
            Some(Resource::Wheat),
            Some(Resource::Wheat),
            Some(Resource::Wheat),
            Some(Resource::Wheat),
            Some(Resource::Wood),
            Some(Resource::Wood),
            Some(Resource::Wood),
            Some(Resource::Wood),
        ];
        hex_resource_queue.shuffle(&mut rng);

        let mut hex_roll_queue = vec![2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12];
        hex_roll_queue.shuffle(&mut rng);

        let mut port_queue = vec![
            Port::Any,
            Port::Any,
            Port::Any,
            Port::Any,
            Port::Only(Resource::Brick),
            Port::Only(Resource::Ore),
            Port::Only(Resource::Sheep),
            Port::Only(Resource::Wheat),
            Port::Only(Resource::Wood),
        ];
        port_queue.shuffle(&mut rng);

        let mut make_hex = |x: u16, y: u16, is_ocean: bool| -> Option<Node> {
            let hex_type = if is_ocean {
                HexType::Ocean
            } else {
                if let Some(resource) = hex_resource_queue.pop()? {
                    let roll = hex_roll_queue.pop()?;
                    HexType::WithResource(HexResource { resource, roll })
                } else {
                    HexType::Desert
                }
            };
            Some(Node::Hex(Hex::new(Index::new(x, y), hex_type)))
        };

        let make_junction = |x: u16, y: u16, port_index: Option<usize>| -> Option<Node> {
            Some(Node::Junction(Junction::new(
                Index::new(x, y),
                match port_index {
                    None => None,
                    Some(i) => Some(port_queue[i]),
                },
            )))
        };

        let make_road =
            |x: u16, y: u16| -> Option<Node> { Some(Node::Road(Road::new(Index::new(x, y)))) };

        nodes[0][6] = make_hex(0, 6, true);
        nodes[0][10] = make_hex(0, 10, true);
        nodes[0][14] = make_hex(0, 14, true);
        nodes[0][18] = make_hex(0, 18, true);

        nodes[1][8] = make_junction(1, 8, Some(0));
        nodes[1][12] = make_junction(1, 12, Some(1));
        nodes[1][16] = make_junction(1, 16, None);

        nodes[2][7] = make_road(2, 7);
        nodes[2][9] = make_road(2, 9);
        nodes[2][11] = make_road(2, 11);
        nodes[2][13] = make_road(2, 13);
        nodes[2][15] = make_road(2, 15);
        nodes[2][17] = make_road(2, 17);

        nodes[3][6] = make_junction(3, 6, Some(0));
        nodes[3][10] = make_junction(3, 10, None);
        nodes[3][14] = make_junction(3, 14, Some(1));
        nodes[3][18] = make_junction(3, 18, None);

        nodes[4][4] = make_hex(4, 4, true);
        nodes[4][6] = make_road(4, 6);
        nodes[4][8] = make_hex(4, 8, false);
        nodes[4][10] = make_road(4, 10);
        nodes[4][12] = make_hex(4, 12, false);
        nodes[4][14] = make_road(4, 14);
        nodes[4][16] = make_hex(4, 16, false);
        nodes[4][18] = make_road(4, 18);
        nodes[4][20] = make_hex(4, 20, true);

        nodes[5][6] = make_junction(5, 6, None);
        nodes[5][10] = make_junction(5, 10, None);
        nodes[5][14] = make_junction(5, 14, None);
        nodes[5][18] = make_junction(5, 18, Some(2));

        nodes[6][5] = make_road(6, 5);
        nodes[6][7] = make_road(6, 7);
        nodes[6][9] = make_road(6, 9);
        nodes[6][11] = make_road(6, 11);
        nodes[6][13] = make_road(6, 13);
        nodes[6][15] = make_road(6, 15);
        nodes[6][13] = make_road(6, 13);
        nodes[6][15] = make_road(6, 15);
        nodes[6][17] = make_road(6, 17);
        nodes[6][19] = make_road(6, 19);

        nodes[7][4] = make_junction(7, 4, Some(8));
        nodes[7][8] = make_junction(7, 8, None);
        nodes[7][12] = make_junction(7, 12, None);
        nodes[7][16] = make_junction(7, 16, None);
        nodes[7][20] = make_junction(7, 20, Some(2));

        nodes[8][2] = make_hex(8, 2, true);
        nodes[8][4] = make_road(8, 4);
        nodes[8][6] = make_hex(8, 6, false);
        nodes[8][8] = make_road(8, 8);
        nodes[8][10] = make_hex(8, 10, false);
        nodes[8][12] = make_road(8, 12);
        nodes[8][14] = make_hex(8, 14, false);
        nodes[8][16] = make_road(8, 16);
        nodes[8][18] = make_hex(8, 18, false);
        nodes[8][20] = make_road(8, 20);
        nodes[8][22] = make_hex(8, 22, true);

        nodes[9][4] = make_junction(9, 4, Some(8));
        nodes[9][8] = make_junction(9, 8, None);
        nodes[9][12] = make_junction(9, 12, None);
        nodes[9][16] = make_junction(9, 16, None);
        nodes[9][20] = make_junction(9, 20, None);

        nodes[10][3] = make_road(10, 3);
        nodes[10][5] = make_road(10, 5);
        nodes[10][7] = make_road(10, 7);
        nodes[10][9] = make_road(10, 9);
        nodes[10][11] = make_road(10, 11);
        nodes[10][13] = make_road(10, 13);
        nodes[10][11] = make_road(10, 11);
        nodes[10][13] = make_road(10, 13);
        nodes[10][15] = make_road(10, 15);
        nodes[10][17] = make_road(10, 17);
        nodes[10][19] = make_road(10, 19);
        nodes[10][21] = make_road(10, 21);

        nodes[11][2] = make_junction(11, 2, None);
        nodes[11][6] = make_junction(11, 6, None);
        nodes[11][10] = make_junction(11, 10, None);
        nodes[11][14] = make_junction(11, 14, None);
        nodes[11][18] = make_junction(11, 18, None);
        nodes[11][22] = make_junction(11, 22, Some(3));

        nodes[12][0] = make_hex(12, 0, true);
        nodes[12][2] = make_road(12, 2);
        nodes[12][4] = make_hex(12, 4, false);
        nodes[12][6] = make_road(12, 6);
        nodes[12][8] = make_hex(12, 8, false);
        nodes[12][10] = make_road(12, 10);
        nodes[12][12] = make_hex(12, 12, false);
        nodes[12][14] = make_road(12, 14);
        nodes[12][16] = make_hex(12, 16, false);
        nodes[12][18] = make_road(12, 18);
        nodes[12][20] = make_hex(12, 20, false);
        nodes[12][22] = make_road(12, 22);
        nodes[12][24] = make_hex(12, 24, true);

        nodes[13][2] = make_junction(13, 2, None);
        nodes[13][6] = make_junction(13, 6, None);
        nodes[13][10] = make_junction(13, 10, None);
        nodes[13][14] = make_junction(13, 14, None);
        nodes[13][18] = make_junction(13, 18, None);
        nodes[13][22] = make_junction(13, 22, Some(3));

        nodes[14][3] = make_road(14, 3);
        nodes[14][5] = make_road(14, 5);
        nodes[14][7] = make_road(14, 7);
        nodes[14][9] = make_road(14, 9);
        nodes[14][11] = make_road(14, 11);
        nodes[14][13] = make_road(14, 13);
        nodes[14][11] = make_road(14, 11);
        nodes[14][13] = make_road(14, 13);
        nodes[14][15] = make_road(14, 15);
        nodes[14][17] = make_road(14, 17);
        nodes[14][19] = make_road(14, 19);
        nodes[14][21] = make_road(14, 21);

        nodes[15][4] = make_junction(15, 4, Some(7));
        nodes[15][8] = make_junction(15, 8, None);
        nodes[15][12] = make_junction(15, 12, None);
        nodes[15][16] = make_junction(15, 16, None);
        nodes[15][20] = make_junction(15, 20, None);

        nodes[16][2] = make_hex(16, 2, true);
        nodes[16][4] = make_road(16, 4);
        nodes[16][6] = make_hex(16, 6, false);
        nodes[16][8] = make_road(16, 8);
        nodes[16][10] = make_hex(16, 10, false);
        nodes[16][12] = make_road(16, 12);
        nodes[16][14] = make_hex(16, 14, false);
        nodes[16][16] = make_road(16, 16);
        nodes[16][18] = make_hex(16, 18, false);
        nodes[16][20] = make_road(16, 20);
        nodes[16][22] = make_hex(16, 22, true);

        nodes[17][4] = make_junction(17, 4, Some(7));
        nodes[17][8] = make_junction(17, 8, None);
        nodes[17][12] = make_junction(17, 12, None);
        nodes[17][16] = make_junction(17, 16, None);
        nodes[17][20] = make_junction(17, 20, Some(4));

        nodes[18][5] = make_road(18, 5);
        nodes[18][7] = make_road(18, 7);
        nodes[18][9] = make_road(18, 9);
        nodes[18][11] = make_road(18, 11);
        nodes[18][13] = make_road(18, 13);
        nodes[18][15] = make_road(18, 15);
        nodes[18][13] = make_road(18, 13);
        nodes[18][15] = make_road(18, 15);
        nodes[18][17] = make_road(18, 17);
        nodes[18][19] = make_road(18, 19);

        nodes[19][6] = make_junction(19, 6, None);
        nodes[19][10] = make_junction(19, 10, None);
        nodes[19][14] = make_junction(19, 14, None);
        nodes[19][18] = make_junction(19, 18, Some(4));

        nodes[20][4] = make_hex(20, 4, true);
        nodes[20][6] = make_road(20, 6);
        nodes[20][8] = make_hex(20, 8, false);
        nodes[20][10] = make_road(20, 10);
        nodes[20][12] = make_hex(20, 12, false);
        nodes[20][14] = make_road(20, 14);
        nodes[20][16] = make_hex(20, 16, false);
        nodes[20][18] = make_road(20, 18);
        nodes[20][20] = make_hex(20, 20, true);

        nodes[21][6] = make_junction(21, 6, Some(6));
        nodes[21][10] = make_junction(21, 10, None);
        nodes[21][14] = make_junction(21, 14, Some(5));
        nodes[21][18] = make_junction(21, 18, None);

        nodes[22][7] = make_road(22, 7);
        nodes[22][9] = make_road(22, 9);
        nodes[22][11] = make_road(22, 11);
        nodes[22][13] = make_road(22, 13);
        nodes[22][15] = make_road(22, 15);
        nodes[22][17] = make_road(22, 17);

        nodes[23][8] = make_junction(23, 8, Some(6));
        nodes[23][12] = make_junction(23, 12, Some(5));
        nodes[23][16] = make_junction(23, 16, None);

        nodes[24][6] = make_hex(24, 6, true);
        nodes[24][10] = make_hex(24, 10, true);
        nodes[24][14] = make_hex(24, 14, true);
        nodes[24][18] = make_hex(24, 18, true);

        nodes
    }

    pub fn dev_cards<R: Rng>(mut rng: &mut R) -> Vec<DevCard> {
        let mut dev_cards = vec![
            DevCard::Knight,
            DevCard::Knight,
            DevCard::Knight,
            DevCard::Knight,
            DevCard::Knight,
            DevCard::Knight,
            DevCard::Knight,
            DevCard::Knight,
            DevCard::Knight,
            DevCard::Knight,
            DevCard::Knight,
            DevCard::Knight,
            DevCard::Knight,
            DevCard::Knight,
            DevCard::Monopoly,
            DevCard::Monopoly,
            DevCard::RoadBuilding,
            DevCard::RoadBuilding,
            DevCard::VictoryPoint,
            DevCard::VictoryPoint,
            DevCard::VictoryPoint,
            DevCard::VictoryPoint,
            DevCard::VictoryPoint,
            DevCard::YearOfPlenty,
            DevCard::YearOfPlenty,
        ];
        dev_cards.shuffle(&mut rng);
        dev_cards
    }
}
