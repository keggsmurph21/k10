use rand::Rng;

use serde::{Deserialize, Serialize};

use std::collections::HashSet;
use std::convert::TryFrom;
use std::convert::TryInto;

pub mod boards;

pub mod index;
pub use index::Index;

pub mod hex;
pub use hex::{Hex, HexType};

pub mod junction;
pub use junction::Junction;

pub mod port;
pub use port::Port;

pub mod road;
pub use road::{Road, RoadOrientation};

use super::player::PlayerId;
use super::resource::{Resource, ResourceCounts};

#[derive(Debug, Deserialize, Serialize)]
pub enum Node {
    Hex(Hex),
    Junction(Junction),
    Road(Road),
}

#[derive(Clone, Copy, Debug, Deserialize, Serialize)]
pub enum BoardType {
    Standard,
}

pub type Nodes = Vec<Vec<Option<Node>>>;

enum NeighborType {
    HexJunction,
    JunctionJunction,
    JunctionRoad,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Board {
    pub board_type: BoardType,
    nodes: Nodes,
}

impl Board {
    pub fn new<R: Rng>(board_type: BoardType, mut rng: &mut R) -> Board {
        match board_type {
            BoardType::Standard => Board {
                nodes: boards::standard::nodes(&mut rng),
                board_type,
            },
        }
    }

    pub fn get_node(&self, index: &Index) -> Option<&Node> {
        self.nodes
            .get(usize::from(index.x))?
            .get(usize::from(index.y))?
            .as_ref()
    }

    pub fn get_hex(&self, index: &Index) -> Option<&Hex> {
        match self.get_node(index) {
            Some(Node::Hex(h)) => Some(&h),
            _ => None,
        }
    }

    pub fn get_junction(&self, index: &Index) -> Option<&Junction> {
        match self.get_node(index) {
            Some(Node::Junction(j)) => Some(&j),
            _ => None,
        }
    }

    pub fn get_road(&self, index: &Index) -> Option<&Road> {
        match self.get_node(index) {
            Some(Node::Road(r)) => Some(&r),
            _ => None,
        }
    }

    fn get_node_mut(&mut self, index: &Index) -> Option<&mut Node> {
        self.nodes
            .get_mut(usize::from(index.x))?
            .get_mut(usize::from(index.y))?
            .as_mut()
    }

    pub fn get_junction_mut(&mut self, index: &Index) -> Option<&mut Junction> {
        match self.get_node_mut(index) {
            Some(Node::Junction(j)) => Some(j),
            _ => None,
        }
    }

    pub fn get_road_mut(&mut self, index: &Index) -> Option<&mut Road> {
        match self.get_node_mut(index) {
            Some(Node::Road(j)) => Some(j),
            _ => None,
        }
    }

    pub fn junctions(&self) -> Vec<&Junction> {
        self.nodes
            .iter()
            .flat_map(|row| row.iter())
            .map(|node| match node {
                Some(Node::Junction(j)) => Some(j),
                _ => None,
            })
            .filter_map(|n| n)
            .collect()
    }

    pub fn hexes(&self) -> Vec<&Hex> {
        self.nodes
            .iter()
            .flat_map(|row| row.iter())
            .map(|node| match node {
                Some(Node::Hex(h)) => Some(h),
                _ => None,
            })
            .filter_map(|n| n)
            .collect()
    }

    pub fn can_build_road_thru(&self, me: &PlayerId, index: &Index) -> bool {
        match self.get_junction(&index) {
            Some(Junction { owner: None, .. }) => true,
            Some(Junction {
                owner: Some(owner), ..
            }) => owner == me,
            _ => false,
        }
    }

    fn longest_path_starting_from(
        &self,
        origin: &Index,
        last_junction: Option<&Index>,
        me: PlayerId,
        my_roads: &HashSet<Index>,
        mut visited: HashSet<Index>,
    ) -> u8 {
        visited.insert(*origin);

        // Walk each of our (junction) neighbors
        self.junction_neighbors_of_road(origin)
            .into_iter()
            //
            // Can't connect roads if someone has settled in
            // the middle!
            .filter(|j| j.owner.unwrap_or(me) == me)
            //
            // Walk each (road) neighbor of each (junction)
            // neighbor
            .map(|j| {
                self.road_neighbors_of_junction(&j.index)
                    .into_iter()
                    .map(move |r| (j.index, r))
            })
            .flatten()
            //
            // Don't backtrack over `last_junction`
            .filter(|(j, _)| {
                last_junction
                    .map(|last_junction| j != last_junction)
                    .unwrap_or(true)
            })
            //
            // Only consider roads that I own
            .filter(|(_, r)| my_roads.contains(&r.index))
            //
            // Only go down each path once
            .filter(|(_, r)| !visited.contains(&r.index))
            //
            // (debugging)
            //.map(|(j, r)| {
            //    println!(
            //        "visiting {:?} from {:?}, visited {:?} {:?}",
            //        r,
            //        last_junction,
            //        visited.len(),
            //        visited
            //    );
            //    (j, r)
            //})
            //
            // Recurse!
            .map(|(j, r)| {
                self.longest_path_starting_from(
                    &r.index,
                    Some(&j),
                    me,
                    my_roads,
                    visited.iter().copied().collect(),
                )
            })
            .max()
            //
            // If we didn't have any neighbors that we haven't
            // already visisted, then the size of *our* visited
            // set is the largest.
            .unwrap_or(visited.len().try_into().unwrap())
    }

    pub fn longest_road(&self, me: PlayerId, my_roads: &HashSet<Index>) -> u8 {
        // For now, just be super naive and calculate all possible combos
        my_roads
            .iter()
            //
            // (debugging)
            //.map(|r| {
            //    println!("starting from {:?} ...", r);
            //    r
            //})
            //
            // Starting from each of our roads, calculate the longest
            // path outwards from that node
            .map(|r| self.longest_path_starting_from(r, None, me, my_roads, HashSet::new()))
            .max()
            //
            // No roads at all?
            .unwrap_or(0)
    }

    pub fn hex_neighbors_of_junction(&self, origin: &Index) -> HashSet<&Hex> {
        self.neighbors(&origin, NeighborType::HexJunction, |h| self.get_hex(&h))
    }

    pub fn junction_neighbors_of_junction(&self, origin: &Index) -> HashSet<&Junction> {
        self.neighbors(&origin, NeighborType::JunctionJunction, |j| {
            self.get_junction(&j)
        })
    }

    pub fn junction_neighbors_of_hex(&self, origin: &Index) -> HashSet<&Junction> {
        self.neighbors(&origin, NeighborType::HexJunction, |j| {
            self.get_junction(&j)
        })
    }

    pub fn junction_neighbors_of_road(&self, origin: &Index) -> HashSet<&Junction> {
        self.neighbors(&origin, NeighborType::JunctionRoad, |j| {
            self.get_junction(&j)
        })
    }

    pub fn road_neighbors_of_junction(&self, origin: &Index) -> HashSet<&Road> {
        self.neighbors(&origin, NeighborType::JunctionRoad, |r| self.get_road(&r))
    }

    fn neighbors<N, F>(&self, origin: &Index, neighbor_type: NeighborType, f: F) -> HashSet<N>
    where
        N: std::cmp::Eq,
        N: std::hash::Hash,
        F: Fn(Index) -> Option<N>,
    {
        let x = i32::from(origin.x);
        let y = i32::from(origin.y);
        match neighbor_type {
            NeighborType::HexJunction => vec![(-3, 0), (-1, 2), (1, 2), (3, 0), (1, -2), (-1, -2)],
            NeighborType::JunctionJunction => {
                vec![(-2, 0), (-2, 2), (2, 2), (2, 0), (2, -2), (-2, -2)]
            }
            NeighborType::JunctionRoad => vec![(-1, 0), (-1, 1), (1, 1), (1, 0), (1, -1), (-1, -1)],
        }
        .into_iter()
        .map(|(dx, dy)| (x + dx, y + dy))
        .filter(|(x, y)| x >= &0 && y >= &0)
        .map(|(x, y)| (u16::try_from(x).unwrap(), u16::try_from(y).unwrap()))
        .map(|(x, y)| Index::new(x, y))
        .map(f)
        .filter_map(|n| n)
        .collect()
    }
}

impl std::fmt::Display for Board {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        for row in &self.nodes {
            for col in row {
                let ch = match col {
                    None => " ",
                    Some(Node::Junction(_)) => ".",
                    Some(Node::Hex(Hex {
                        hex_type: HexType::Ocean,
                        ..
                    })) => "~",
                    Some(Node::Hex(Hex {
                        hex_type: HexType::Desert,
                        ..
                    })) => "d",
                    Some(Node::Hex(Hex {
                        hex_type:
                            HexType::WithResource {
                                resource: Resource::Brick,
                                ..
                            },
                        ..
                    })) => "b",
                    Some(Node::Hex(Hex {
                        hex_type:
                            HexType::WithResource {
                                resource: Resource::Ore,
                                ..
                            },
                        ..
                    })) => "o",
                    Some(Node::Hex(Hex {
                        hex_type:
                            HexType::WithResource {
                                resource: Resource::Sheep,
                                ..
                            },
                        ..
                    })) => "s",
                    Some(Node::Hex(Hex {
                        hex_type:
                            HexType::WithResource {
                                resource: Resource::Wheat,
                                ..
                            },
                        ..
                    })) => "t",
                    Some(Node::Hex(Hex {
                        hex_type:
                            HexType::WithResource {
                                resource: Resource::Wood,
                                ..
                            },
                        ..
                    })) => "w",
                    Some(Node::Road(Road {
                        orientation: RoadOrientation::BackSlash,
                        ..
                    })) => "\\",
                    Some(Node::Road(Road {
                        orientation: RoadOrientation::ForwardSlash,
                        ..
                    })) => "/",
                    Some(Node::Road(Road {
                        orientation: RoadOrientation::Pipe,
                        ..
                    })) => "|",
                };
                write!(f, "{}", ch)?;
            }
            write!(f, "\n")?;
        }
        Ok(())
    }
}

#[cfg(test)]
mod test {
    use super::*;

    use rand;

    const ME: u64 = 100;
    const YOU: u64 = 101;

    fn check_longest_road(my_roads: Vec<(u16, u16)>, your_junctions: Vec<(u16, u16)>) -> u8 {
        let mut b = Board::new(BoardType::Standard, &mut rand::thread_rng());
        let my_roads = my_roads
            .into_iter()
            .map(|(x, y)| Index::new(x, y))
            .collect();
        for road in &my_roads {
            b.get_road_mut(road).unwrap().owner = Some(ME);
        }
        for (x, y) in your_junctions {
            b.get_junction_mut(&Index::new(x, y)).unwrap().owner = Some(YOU);
        }
        b.longest_road(ME, &my_roads)
    }

    #[test]
    fn test_longest_road() {
        assert_eq!(check_longest_road(vec![], vec![]), 0);
        assert_eq!(check_longest_road(vec![(2, 7)], vec![]), 1);
        assert_eq!(check_longest_road(vec![(2, 7), (2, 9)], vec![]), 2);
        assert_eq!(check_longest_road(vec![(2, 7), (6, 9)], vec![]), 1);
        assert_eq!(check_longest_road(vec![(2, 7), (2, 9)], vec![(1, 8)]), 1);
        assert_eq!(
            check_longest_road(
                vec![(2, 7), (2, 9), (4, 10), (4, 6), (6, 7), (6, 9)],
                vec![]
            ),
            6
        );
        assert_eq!(
            check_longest_road(
                vec![
                    (2, 7),
                    (2, 9),
                    (4, 10),
                    (4, 6),
                    (6, 11),
                    (6, 5),
                    (6, 7),
                    (6, 9),
                ],
                vec![]
            ),
            7
        );
    }
}
