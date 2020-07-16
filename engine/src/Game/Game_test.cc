#include "Board/Store.h"
#include "Game/Game.h"
#include "Scenario/Store.h"
#include "Test/catch.h"

namespace k10engine::Game {

static PlayerId s_next_player_id = 1;

TEST_CASE("Game serialization", "[Game]")
{
    SECTION("Single")
    {
        auto s = Scenario::Store::the().by_name(Scenario::Name::Single);
        auto p = Scenario::Parameters{ Scenario::IterationType::Fixed,
                                       Scenario::IterationType::Fixed,
                                       Scenario::IterationType::Fixed,
                                       Scenario::IterationType::Fixed,
                                       1,
                                       3 };
        auto expected = Game::Game::initialize(s_next_player_id++, s, p);

        ByteBuffer buf;
        Encoder encoder(buf);
        encoder << *expected;

        Decoder decoder(buf);
        Game* actual = nullptr;
        REQUIRE(decoder.decode(actual));
        REQUIRE(actual != nullptr);
        REQUIRE(*actual == *expected);

        delete expected;
        delete actual;
    }

    SECTION("Triple")
    {
        auto s = Scenario::Store::the().by_name(Scenario::Name::Triple);
        auto p = Scenario::Parameters{ Scenario::IterationType::Fixed,
                                       Scenario::IterationType::Fixed,
                                       Scenario::IterationType::Fixed,
                                       Scenario::IterationType::Fixed,
                                       2,
                                       3 };
        auto expected = Game::Game::initialize(s_next_player_id++, s, p);

        ByteBuffer buf;
        Encoder encoder(buf);
        encoder << *expected;

        Decoder decoder(buf);
        Game* actual = nullptr;
        REQUIRE(decoder.decode(actual));
        REQUIRE(actual != nullptr);
        REQUIRE(*actual == *expected);

        delete expected;
        delete actual;
    }

    SECTION("Standard")
    {
        auto s = Scenario::Store::the().by_name(Scenario::Name::Standard);
        auto p = Scenario::Parameters{ Scenario::IterationType::Fixed,
                                       Scenario::IterationType::Fixed,
                                       Scenario::IterationType::Fixed,
                                       Scenario::IterationType::Fixed,
                                       4,
                                       10 };
        auto expected = Game::Game::initialize(s_next_player_id++, s, p);

        ByteBuffer buf;
        Encoder encoder(buf);
        encoder << *expected;

        Decoder decoder(buf);
        Game* actual = nullptr;
        REQUIRE(decoder.decode(actual));
        REQUIRE(actual != nullptr);
        REQUIRE(*actual == *expected);

        delete expected;
        delete actual;
    }
}
} // namespace k10engine::Game
