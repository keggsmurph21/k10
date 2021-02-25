use serde::Serialize;
use sqlx::sqlite::SqlitePool;

pub type Password = String;
// sqlx whines about unsigned
pub type UserToken = i64;
pub type UserId = i64;
pub type Username = String;

/// "Private" data that won't leave the server/db.
#[derive(sqlx::FromRow)]
pub struct User {
    pub id: UserId,
    pub username: Username,
    pub password: Password, // TODO: Save a hash instead
}

/// "Public" data viewable by anyone.
#[derive(Clone, Debug, Serialize)]
pub struct UserData {
    pub id: UserId,
    pub name: Username,
}

impl User {
    pub fn data(&self) -> UserData {
        UserData {
            id: self.id,
            name: self.username.clone(),
        }
    }

    pub fn schema() -> &'static str {
        "CREATE TABLE users (
            id INTEGER NOT NULL PRIMARY KEY,
            username TEXT NOT NULL,
            password TEXT NOT NULL
        )"
    }

    pub async fn by_username(
        pool: &SqlitePool,
        username: &String,
    ) -> Result<Option<Self>, sqlx::Error> {
        sqlx::query_as("SELECT * FROM users WHERE username = ?")
            .bind(username)
            .fetch_optional(pool)
            .await
    }

    pub async fn by_id(pool: &SqlitePool, id: &UserId) -> Result<Self, sqlx::Error> {
        sqlx::query_as("SELECT * FROM users WHERE id = ?")
            .bind(id)
            .fetch_one(pool)
            .await
    }

    pub async fn new(
        pool: &SqlitePool,
        username: &String,
        password: &String,
    ) -> Result<Self, sqlx::Error> {
        let res = sqlx::query("INSERT INTO users (username, password) VALUES (?, ?)")
            .bind(username)
            .bind(password)
            .execute(pool)
            .await?;
        Self::by_id(&pool, &res.last_insert_rowid()).await
    }
}
