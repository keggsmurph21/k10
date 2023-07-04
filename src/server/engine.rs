#![allow(dead_code)]

use super::api;
use super::client;
use super::model;
use crate::core;
use std::collections::HashMap;
use std::collections::HashSet;

struct Channel<T: Clone> {
    tx: tokio::sync::broadcast::Sender<T>,
}

impl<T: Clone> Channel<T> {
    pub fn new() -> Self {
        let (tx, _) = tokio::sync::broadcast::channel(4096); // buffer size
        Self { tx }
    }

    pub fn publish(&self, t: T) -> Result<usize, tokio::sync::broadcast::error::SendError<T>> {
        self.tx.send(t)
    }

    pub fn subscribe(&self) -> Result<tokio::sync::broadcast::Receiver<T>, ()> {
        Ok(self.tx.subscribe())
    }
}

#[derive(Clone, Debug, Eq, Hash, PartialEq)]
struct ClientId(usize);

struct EngineState {
    active_games: HashMap<model::game::GameId, model::game::Game>,
    pending_games: HashMap<model::game::GameId, model::pending_game::PendingGame>,
    clients: HashMap<ClientId, client::Client>,
    private_client_channels: HashMap<model::user::UserId, Channel<api::Response>>,
    public_in_game_channels: HashMap<model::game::GameId, Channel<api::InGameResponse>>,
    lobby_channel: Channel<api::LobbyResponse>,
}

impl EngineState {
    pub fn new() -> Self {
        Self {
            active_games: HashMap::new(),
            pending_games: HashMap::new(),
            clients: HashMap::new(),
            private_client_channels: HashMap::new(),
            public_in_game_channels: HashMap::new(),
            lobby_channel: Channel::new(),
        }
    }
}

struct Engine {
    state: EngineState,
}

impl Engine {
    pub fn new() -> Self {
        Self {
            state: EngineState::new(),
        }
    }

    #[allow(unreachable_code)]
    fn authenticate(
        &mut self,
        _client_id: ClientId,
        _req: api::AuthenticateRequest,
    ) -> Result<api::Response, ()> {
        // TODO: do actual authentication...
        self.state
            .lobby_channel
            .publish(api::LobbyResponse::UserConnected(todo!()));
        let _rx = self.state.lobby_channel.subscribe(); // TODO: actually hold onto this rx...

        api::Response::Lobby(api::LobbyResponse::Authenticated(vec![])); // TODO: get current UserData
    }

    #[allow(unused_variables)]
    #[allow(unreachable_code)]
    fn create_game(&mut self, _req: api::CreateGameRequest) -> Result<api::Response, ()> {
        // TODO: do actual creation...
        let game_channel = Channel::<api::InGameResponse>::new();
        let _rx = game_channel.subscribe(); // TODO: actually hold onto this rx...

        let res = api::LobbyResponse::CreatedGame(todo!());
        self.state.lobby_channel.publish(res);

        Ok(api::Response::Lobby(res))
    }

    fn join_game(
        &mut self,
        client_id: ClientId,
        req: api::JoinGameRequest,
    ) -> Result<api::Response, ()> {
        let mut pending_game = self
            .state
            .pending_games
            .get_mut(&req.game_id)
            .expect("game not found");

        if pending_game.users.len() == usize::from(pending_game.num_players) {
            panic!("Game already full!");
        }

        let mut client = self
            .state
            .clients
            .get_mut(&client_id)
            .expect("client not found");

        if pending_game.users.contains(&client.user_data.id) {
            panic!("Already in game!");
        }

        let game_channel = self
            .state
            .public_in_game_channels
            .get(&req.game_id)
            .expect("game not found");

        pending_game.users.insert(client.user_data.id);
        if pending_game.users.len() != usize::from(pending_game.num_players) {
            // We're not ready to start it yet.
            let res = api::LobbyResponse::PlayersChanged(pending_game.clone());
            self.state.lobby_channel.publish(res);
            return Ok(api::Response::Lobby(res));
        }

        // Start it!
        let id = pending_game.id.clone();
        let users = pending_game
            .users
            .iter()
            .cloned()
            .collect::<HashSet<model::user::UserId>>();
        let board_type = pending_game.board_type.clone();
        let victory_points_goal = pending_game.victory_points_goal.clone();
        let game = core::Game::new(
            users.iter().cloned().collect(),
            board_type,
            victory_points_goal,
            &mut rand::thread_rng(),
        )
        .expect("failed to create game");

        pending_game.users.iter().for_each(|user_id| {
            let mut client = self
                .state
                .clients
                .get_mut(&user_id) // uhhh....
                .expect("client not found");

            client.state = client::ClientState::InGame(pending_game.id);

            let res =
                api::InGameResponse::CurrentView(core::view::game::GameView::of(&game.game, &id));

            let client_channel = self
                .state
                .private_client_channels
                .get(&id)
                .expect("channel not found");

            client_channel.publish(res).expect("failed sending");
        });

        let res = api::LobbyResponse::GameStarted(game.id);
        self.state.lobby_channel.publish(res);
        Ok(api::Response::Lobby(res))
    }

    #[allow(unused_variables)]
    #[allow(unreachable_code)]
    fn start_game(&mut self, req: api::StartGameRequest) -> Result<api::Response, ()> {
        // TODO: do actual starting...
        let game_channel = self
            .state
            .public_in_game_channels
            .get(&req.game_id)
            .expect("game not found");
        game_channel.publish(todo!());

        let res = api::LobbyResponse::GameStarted(todo!());
        self.state.lobby_channel.publish(res);

        Ok(api::Response::Lobby(res))
    }

    #[allow(unreachable_code)]
    fn make_move(&mut self, req: api::MakeMoveRequest) -> Result<api::Response, ()> {
        // TODO: do the actual moving...
        let game_channel = self
            .state
            .public_in_game_channels
            .get(&req.game_id)
            .expect("game not found");
        game_channel.publish(api::InGameResponse::MadeMove(todo!()));

        Ok(api::Response::InGame(api::InGameResponse::CurrentView(
            todo!(),
        )))
    }

    pub fn evaluate(
        &mut self,
        client_id: ClientId,
        req: api::Request,
    ) -> Result<api::Response, ()> {
        match req {
            api::Request::Unauthenticated(api::UnauthenticatedRequest::Authenticate(req)) => {
                self.authenticate(client_id, req)
            }
            api::Request::Lobby(api::LobbyRequest::CreateGame(req)) => self.create_game(req),
            api::Request::Lobby(api::LobbyRequest::JoinGame(req)) => self.join_game(client_id, req),
            api::Request::Lobby(api::LobbyRequest::StartGame(req)) => self.start_game(req),
            api::Request::InGame(api::InGameRequest::MakeMove(req)) => self.make_move(req),
        }
    }
}
