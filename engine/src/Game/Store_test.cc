#include <cstdlib>
#include <unistd.h>

#include "Game/Store.h"
#include "Test/catch.h"

TEST_CASE("Store", "[Game][Game.Store]")
{
    char game_store_path_template[] = "/tmp/k10-engine-game-store-XXXXXX";
    const std::string name = mkdtemp(game_store_path_template);
    auto gs = k10engine::Game::Store(name, 1);
    gs.get(100);
    gs.get(0xdeadbeefdeadbeef);
    unlink(name.c_str());
}
