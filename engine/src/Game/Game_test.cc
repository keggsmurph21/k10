#include "Board/Store.h"
#include "Game/Game.h"
#include "Scenario/Store.h"
#include "Test/Utils.h"
#include "Test/catch.h"

namespace k10engine::Game {

TEST_CASE("Game serialization", "[Game]")
{
    SECTION("Single")
    {
        auto expected = make_game(Scenario::Name::Single, 1, 3, false);

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
        auto expected = make_game(Scenario::Name::Triple, 2, 3, false);

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
        auto expected = make_game(Scenario::Name::Standard, 4, 10, false);

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
