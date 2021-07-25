use serde::{Deserialize, Serialize};
use sqlx::sqlite::SqlitePool;
use std::collections::HashSet;

use kcore::board::BoardType;

use super::super::error::ClientError;
use super::game::GameId;
use super::user::UserId;

#[derive(Clone, Debug, Deserialize, Serialize)]
pub struct PendingGame {
    pub id: GameId,
    pub num_players: u8,
    pub users: HashSet<UserId>,
    pub victory_points_goal: u8,
    pub board_type: BoardType,
}

// Just a connector to help with serialization into the DB :^)
#[derive(Debug, sqlx::FromRow)]
struct RawPendingGame {
    pub id: GameId,
    pub num_players: u8,
    pub users: String,
    pub victory_points_goal: u8,
    pub board_type: String,
}

impl PendingGame {
    pub fn schema() -> &'static str {
        "CREATE TABLE pending_games (
            id INTEGER NOT NULL PRIMARY KEY,
            num_players INTEGER NOT NULL,
            users TEXT NOT NULL,
            victory_points_goal INTEGER NOT NULL,
            board_type TEXT NOT NULL
        )"
    }

    fn from_raw(game: RawPendingGame) -> Result<Self, ClientError> {
        Ok(PendingGame {
            id: game.id,
            num_players: game.num_players,
            users: serde_json::from_str(&game.users)?,
            victory_points_goal: game.victory_points_goal,
            board_type: serde_json::from_str(&game.board_type)?,
        })
    }

    fn to_raw(self) -> Result<RawPendingGame, ClientError> {
        Ok(RawPendingGame {
            id: self.id,
            num_players: self.num_players,
            users: serde_json::to_string(&self.users)?,
            victory_points_goal: self.victory_points_goal,
            board_type: serde_json::to_string(&self.board_type)?,
        })
    }

    pub async fn by_id(pool: &SqlitePool, id: &GameId) -> Result<Self, ClientError> {
        let game = sqlx::query_as::<sqlx::Sqlite, RawPendingGame>(
            "SELECT * FROM pending_games WHERE id = ?",
        )
        .bind(id)
        .fetch_one(pool)
        .await?;
        Self::from_raw(game)
    }

    pub async fn new(
        pool: &SqlitePool,
        num_players: u8,
        victory_points_goal: u8,
        board_type: BoardType,
    ) -> Result<Self, ClientError> {
        let res = sqlx::query(
            "INSERT INTO pending_games (num_players, victory_points_goal, users, board_type)
                     VALUES (?, ?, ?, ?)",
        )
        .bind(num_players)
        .bind(victory_points_goal)
        .bind(serde_json::to_string(&HashSet::<UserId>::new())?)
        .bind(serde_json::to_string(&board_type)?)
        .execute(pool)
        .await?;
        Ok(Self::by_id(&pool, &res.last_insert_rowid()).await?)
    }

    pub async fn persist(self, pool: &SqlitePool) -> Result<(), ClientError> {
        let game = self.to_raw()?;
        sqlx::query(
            "UPDATE pending_games
                SET (num_players, victory_points_goal, users, board_type) = (?, ?, ?, ?)
                WHERE id = ?",
        )
        .bind(game.num_players)
        .bind(game.victory_points_goal)
        .bind(game.users)
        .bind(game.board_type)
        .bind(game.id)
        .execute(pool)
        .await?;
        Ok(())
    }

    pub async fn delete(self, pool: &SqlitePool) -> Result<(), ClientError> {
        sqlx::query("DELETE FROM pending_games WHERE id = ?")
            .bind(self.id)
            .execute(pool)
            .await?;
        Ok(())
    }
}
