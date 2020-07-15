#include "Board/Store.h"
#include "Game/Game.h"
#include "Test/catch.h"

namespace k10engine::Game {

TEST_CASE("Game serialization", "[Game]")
{
    auto b = Board::Store::the().by_name(Board::Name::Single);
    auto s = Scenario::get_single_scenario();
    auto p = Scenario::Parameters{ Scenario::IterationType::Fixed,
                                   Scenario::IterationType::Fixed,
                                   Scenario::IterationType::Fixed,
                                   Scenario::IterationType::Fixed,
                                   1,
                                   3 };
    auto expected = Game::Game::initialize(b, *s, p);
    (void)expected;

    ByteBuffer buf;
    Encoder encoder(buf);
    encoder << expected;

    std::cout << "ByteBuffer(" << buf.unread_size() << ") " << buf << std::endl;

    Decoder decoder(buf);
    Game* actual = nullptr;
    REQUIRE(decoder.decode(actual));
    // REQUIRE(expected == actual);
}

} // namespace k10engine::Game
