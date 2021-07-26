use rand::distributions::{Distribution, Uniform};
use rand::Rng;

#[derive(Debug)]
pub struct Die {
    dist: Uniform<u8>,
}

impl Default for Die {
    fn default() -> Self {
        Die {
            dist: Uniform::from(1..=6),
        }
    }
}

impl Die {
    pub fn roll<R: Rng>(&mut self, mut rng: &mut R) -> u8 {
        self.dist.sample(&mut rng)
    }
}
