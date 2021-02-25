/*
use rand;
use rand::Rng;

use serde::{Deserialize, Serialize};
use serde_json::Error;

use std::collections::HashMap;
use std::net::{TcpListener, TcpStream};

use kcore::action;
use kcore::game::Game;

use super::request::{GameId, GameOptions, Message, Request, Response};
use super::user::{Password, UserId, UserToken, Username};

pub type TODO = ();

/// A stupid-simple, single-threaded server.
pub struct GameServer {
    games: HashMap<GameId, Game>,
}

impl GameServer {
    pub fn new() -> Self {
        GameServer {
            games: HashMap::new(),
        }
    }

    pub fn serve<R: Rng>(&mut self, bind_addr: &str, mut rng: &mut R) -> ! {
        let listener = TcpListener::bind(bind_addr).unwrap();
        println!("listening on port {} ...", bind_addr);

        for stream in listener.incoming() {
            match stream {
                Ok(stream) => {
                    println!("New connection from {}", stream.peer_addr().unwrap());
                    match self.handle_client(stream, &mut rng) {
                        Err(e) => panic!(e),
                        _ => (),
                    }
                }
                Err(e) => {
                    println!("Error: {}", e);
                }
            }
        }
        panic!();
    }

    fn handle_register(&mut self, username: Username, password: Password) -> Response {
        println!("username={:?} password={:?}", username, password);
        panic!();
    }

    fn handle_login(&mut self, username: Username, password: Password) -> Response {
        println!("username={:?} password={:?}", username, password);
        panic!();
    }

    fn handle_new_game(&mut self, user: UserId, game: GameId, options: GameOptions) -> Response {
        println!("user={:?} game={:?} options={:?}", user, game, options);
        panic!();
    }

    fn handle_join_game(&mut self, user: UserId, game: GameId) -> Response {
        println!("user={:?} game={:?}", user, game);
        panic!();
    }

    fn handle_leave_game(&mut self, user: UserId, game: GameId) -> Response {
        println!("user={:?} game={:?}", user, game);
        panic!();
    }

    fn handle_make_move(
        &mut self,
        user: UserId,
        token: UserToken,
        action: action::Request,
    ) -> Response {
        println!("user={:?} token={:?} action={:?}", user, token, action);
        panic!();
    }

    fn handle_chat(&mut self, message: Message) -> Response {
        println!("message={:?}", message);
        panic!();
    }

    fn handle_client<R: Rng>(&mut self, stream: TcpStream, mut rng: &mut R) -> Result<(), Error> {
        let mut de = serde_json::Deserializer::from_reader(stream.try_clone().unwrap());
        let res = match Request::deserialize(&mut de) {
            Ok(Request::Register(username, password)) => self.handle_register(username, password),
            Ok(Request::Login(username, password)) => self.handle_login(username, password),
            Ok(Request::NewGame(user, game, options)) => self.handle_new_game(user, game, options),
            Ok(Request::JoinGame(user, game)) => self.handle_join_game(user, game),
            Ok(Request::LeaveGame(user, game)) => self.handle_leave_game(user, game),
            Ok(Request::MakeMove(user, token, action)) => {
                self.handle_make_move(user, token, action)
            }
            Ok(Request::Chat(message)) => self.handle_chat(message),
            Err(e) => {
                println!("Parse error: {:?}", e);
                // Send back the Debug string for this error
                Response::InvalidRequest(format!("{:?}", e))
            }
        };
        serde_json::ser::to_writer(stream, &res)?;
        Ok(())
    }
}
*/
