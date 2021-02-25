pub type GameId = u64;

#[derive(Clone, Debug, Eq, Hash, PartialEq)]
pub enum Channel {
    Lobby,
    Game(GameId),
}
