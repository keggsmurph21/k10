use rand;
use rand::Rng;
use uuid::Uuid;

pub type Token = Uuid;

pub fn new_token() -> Token {
    let bytes = rand::thread_rng().gen::<[u8; 16]>();
    uuid::Builder::from_bytes(bytes).build()
}
