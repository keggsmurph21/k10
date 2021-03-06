#include <cstdlib>
#include <unistd.h>

#include "Board/Store.h"
#include "Scenario/Store.h"
#include "Server/GameCache.h"
#include "Test/Utils.h"
#include "Test/catch.h"

namespace k10engine {

TEST_CASE("GameCache", "[Server][Server.GameCache]")
{
    SECTION("Reading games that don't exist")
    {
        char game_cache_path_template[] = "/tmp/k10-engine-game-cache-XXXXXX";
        const char* path = mkdtemp(game_cache_path_template);
        if (path == nullptr) {
            perror("mkdtemp");
            assert(false);
        }

        auto cache = Server::GameCache(path, 1);
        REQUIRE(cache.get(100) == nullptr);
        REQUIRE(cache.get(0xdeadbeefdeadbeef) == nullptr);

        if (rmdir(path) != 0) {
            perror("rmdir");
            assert(false);
        }
    }

    SECTION("Storing a game to disk")
    {
        char game_cache_path_template[] = "/tmp/k10-engine-game-cache-XXXXXX";
        const char* path = mkdtemp(game_cache_path_template);
        if (path == nullptr) {
            perror("mkdtemp");
            assert(false);
        }

        auto cache = Server::GameCache(path, 1);

        // Need to be careful with pointer ownership here, since the Server::GameCache expects
        // to be able to "delete" at will.
        const auto get_game = [](Game::PlayerId owner_id = 0) -> Game::Game* {
            return make_game(Scenario::Name::Single, 1, 3, true, owner_id);
        };

        auto* expected_game_0 = get_game();
        cache.set(0, get_game(expected_game_0->owner()));

        // force a cache eviction on expected_game_0
        auto* expected_game_1 = get_game();
        cache.set(1, get_game(expected_game_1->owner()));

        // check that expected_game_1 is still good to go
        auto* actual_game_1 = cache.get(1);
        REQUIRE(actual_game_1 != nullptr);
        REQUIRE(*actual_game_1 == *expected_game_1);

        // load in expected_game_0 from disk ...
        auto* actual_game_0 = cache.get(0);
        REQUIRE(actual_game_0 != nullptr);
        REQUIRE(*actual_game_0 == *expected_game_0);

        // check that expected_game_1 is still good to go
        actual_game_1 = cache.get(1);
        REQUIRE(actual_game_1 != nullptr);
        REQUIRE(*actual_game_1 == *expected_game_1);

        delete expected_game_0;
        delete expected_game_1;

        // FIXME: Add an "rm -rf" implementation ...
        // if (rmdir(path) != 0) {
        // perror("rmdir");
        // assert(false);
        //}
    }
}

} // namespace k10engine
