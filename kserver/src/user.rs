// These are the raw types we get over the wire
pub type Password = String;
// sqlx whines about unsigned
pub type UserToken = i64;
pub type UserId = i64;
pub type Username = String;

#[derive(sqlx::FromRow)]
pub struct User {
    pub id: UserId,
    pub username: Username,
    pub password: Password, // TODO: Save a hash instead
}
