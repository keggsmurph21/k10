use crate::core::action::Response;
use crate::core::GameView;
use crate::server::model::game::GameId;
use crate::server::model::pending_game::PendingGame;
use crate::server::model::user::UserData;
use crate::server::model::user::UserId;

use super::me::Me;

#[derive(Debug)]
pub enum CurrentGame {
    None,
    Pending(GameId),
    View(GameView),
}

pub type Users = std::collections::HashMap<UserId, UserData>;

pub type AvailableSlots = u8;
pub type Games = std::collections::HashMap<GameId, AvailableSlots>;

#[derive(Debug)]
pub struct State {
    me: Me,
    pub(super) authenticated: bool,
    current_game: CurrentGame,
    users: Users,
    pending_games: Games,
}

impl State {
    pub fn new(me: Me) -> Self {
        Self {
            me,
            authenticated: false,
            current_game: CurrentGame::None,
            users: Users::new(),
            pending_games: Games::new(),
        }
    }

    pub fn authenticate(&mut self, users: Vec<UserData>) {
        self.users = users.into_iter().map(|user| (user.id, user)).collect();
        self.authenticated = true;
    }

    pub fn user_connected(&mut self, user: UserData) {
        self.users.insert(user.id, user);
    }

    pub fn user_disconnected(&mut self, user: UserData) {
        // TODO: Is this message ever getting sent by the server?
        self.users.remove(&user.id);
    }

    pub fn update_pending_game(&mut self, pending_game: PendingGame) {
        let in_game = pending_game.users.contains(&self.me.id);
        if in_game {
            self.current_game = CurrentGame::Pending(pending_game.id);
        } else {
            let available_slots = pending_game.num_players - pending_game.users.len() as u8;
            self.pending_games.insert(pending_game.id, available_slots);
        }
    }

    pub fn remove_pending_game(&mut self, game_id: &GameId) {
        self.pending_games.remove(game_id);
    }

    pub fn update_view(&mut self, view: GameView) {
        self.current_game = CurrentGame::View(view);
    }

    pub fn made_move(&mut self, res: Response) {
        eprintln!("Made move: {:?}", res);
        // TODO!
    }
}
