#include "Board/Store.h"
#include "Game/Game.h"
#include "Scenario/Store.h"
#include "Test/Utils.h"
#include "Test/catch.h"

namespace k10engine::Game {

TEST_CASE("Game pre-start actions", "[Game][X]")
{
    SECTION("Single")
    {
        PlayerId p1 = s_next_player_id;
        auto g = make_game(Scenario::Name::Single, 1, 3, false);

        for (const auto* player : g->players())
            REQUIRE(player->id() == 0);

        REQUIRE(!g->has_started());
        REQUIRE(g->num_joined() == 1);
        REQUIRE(!g->can_join(p1));
        REQUIRE(!g->join(p1));
        REQUIRE(!g->can_leave(p1));
        REQUIRE(!g->leave(p1));
        REQUIRE(g->can_start());
        REQUIRE(g->start());
        REQUIRE(g->num_joined() == 0);
        REQUIRE(g->has_started());

        for (const auto* player : g->players())
            REQUIRE(player->id() != 0);

        delete g;
    }

    SECTION("Triple")
    {
        PlayerId p1 = s_next_player_id;
        auto g = make_game(Scenario::Name::Triple, 2, 3, false);
        PlayerId p2 = s_next_player_id++;

        for (const auto* player : g->players())
            REQUIRE(player->id() == 0);

        REQUIRE(g->num_joined() == 1);
        REQUIRE(!g->can_start());
        REQUIRE(!g->start());

        REQUIRE(!g->can_leave(p2));
        REQUIRE(!g->leave(p2));

        REQUIRE(g->can_join(p2));
        REQUIRE(g->join(p2));
        REQUIRE(g->num_joined() == 2);
        REQUIRE(g->can_start());

        REQUIRE(!g->can_leave(p1));
        REQUIRE(!g->leave(p1));
        REQUIRE(g->can_leave(p2));
        REQUIRE(g->leave(p2));
        REQUIRE(g->num_joined() == 1);
        REQUIRE(!g->can_start());

        REQUIRE(g->join(p2));
        REQUIRE(g->can_start());
        REQUIRE(g->start());
        REQUIRE(g->has_started());

        for (const auto* player : g->players())
            REQUIRE(player->id() != 0);

        delete g;
    }
}

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
