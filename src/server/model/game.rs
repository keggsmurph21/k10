use sqlx::sqlite::SqlitePool;
use std::collections::HashSet;

use crate::core;

use super::super::error::ClientError;
use super::user::UserId;

pub type GameId = i64;

#[derive(Debug)]
pub struct Game {
    pub id: GameId,
    pub users: HashSet<UserId>,
    pub game: core::Game,
}

#[derive(Debug, sqlx::FromRow)]
struct RawGame {
    pub id: GameId,
    pub users: String,
    pub game: String,
}

impl Game {
    pub fn schema() -> &'static str {
        "CREATE TABLE games (
            id INTEGER NOT NULL PRIMARY KEY,
            users TEXT NOT NULL,
            game TEXT NOT NULL
        )"
    }

    fn from_raw(game: RawGame) -> Result<Self, ClientError> {
        Ok(Game {
            id: game.id,
            users: serde_json::from_str(&game.users)?,
            game: serde_json::from_str(&game.game)?,
        })
    }

    fn to_raw(self) -> Result<RawGame, ClientError> {
        Ok(RawGame {
            id: self.id,
            users: serde_json::to_string(&self.users)?,
            game: serde_json::to_string(&self.game)?,
        })
    }

    pub async fn by_id(pool: &SqlitePool, id: &GameId) -> Result<Self, ClientError> {
        let game = sqlx::query_as::<sqlx::Sqlite, RawGame>("SELECT * FROM games WHERE id = ?")
            .bind(id)
            .fetch_one(pool)
            .await?;
        Self::from_raw(game)
    }

    pub async fn new(
        pool: &SqlitePool,
        id: GameId,
        users: HashSet<UserId>,
        game: core::Game,
    ) -> Result<Self, ClientError> {
        let game = Game { id, users, game };
        let game = game.to_raw()?;
        sqlx::query("INSERT INTO games (id, users, game) VALUES (?, ?, ?)")
            .bind(game.id)
            .bind(game.users)
            .bind(game.game)
            .execute(pool)
            .await?;
        Ok(Self::by_id(&pool, &id).await?)
    }

    pub async fn persist(self, pool: &SqlitePool) -> Result<(), ClientError> {
        let game = self.to_raw()?;
        sqlx::query(
            "UPDATE games
                    SET users = ?
                      , game = ?
                    WHERE id = ?",
        )
        .bind(game.users)
        .bind(game.game)
        .bind(game.id)
        .execute(pool)
        .await?;
        Ok(())
    }
}
