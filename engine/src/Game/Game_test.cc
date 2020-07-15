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
    encoder << *expected;

    std::cout << "ByteBuffer(" << buf.unread_size() << ") " << buf << std::endl;

    Decoder decoder(buf);
    Game* actual = nullptr;
    REQUIRE(decoder.decode(actual));
    REQUIRE(actual != nullptr);

    REQUIRE(actual->can_steal() == expected->can_steal());
    REQUIRE(actual->has_rolled() == expected->has_rolled());
    REQUIRE(actual->is_game_over() == expected->is_game_over());
    REQUIRE(actual->is_trade_accepted() == expected->is_trade_accepted());
    REQUIRE(actual->is_first_round() == expected->is_first_round());
    REQUIRE(actual->is_second_round() == expected->is_second_round());
    REQUIRE(actual->is_roll_seven() == expected->is_roll_seven());
    REQUIRE(actual->should_wait_for_discard() == expected->should_wait_for_discard());
    REQUIRE(actual->should_wait_for_trade() == expected->should_wait_for_trade());

    REQUIRE(actual->num_trades_offered_this_turn() == expected->num_trades_offered_this_turn());
    REQUIRE(actual->has_current_trade() == expected->has_current_trade());
    REQUIRE(actual->current_trade() == expected->current_trade());
    REQUIRE(actual->robber_location()->index() == expected->robber_location()->index());

    REQUIRE(actual->get_dice_total() == expected->get_dice_total());

    REQUIRE(actual->turn() == expected->turn());
    REQUIRE(actual->get_round() == expected->get_round());

    REQUIRE(actual->largest_army() == expected->largest_army());
    REQUIRE(actual->longest_road() == expected->longest_road());

    if (actual->has_largest_army() == nullptr) {
        REQUIRE(expected->has_largest_army() == nullptr);
    } else {
        REQUIRE(actual->has_largest_army()->index() == expected->has_largest_army()->index());
    }

    if (actual->has_longest_road() == nullptr) {
        REQUIRE(expected->has_longest_road() == nullptr);
    } else {
        REQUIRE(actual->has_longest_road()->index() == expected->has_longest_road()->index());
    }

    for (auto i = 0; i < 4; ++i) {
        REQUIRE(actual->num_built(static_cast<Building>(i)) == expected->num_built(static_cast<Building>(i)));
    }

    REQUIRE(Board::Store::the().name_of(actual->graph()) == Board::Store::the().name_of(expected->graph()));
    REQUIRE(actual->scenario() == expected->scenario());

    REQUIRE(actual->current_player().index() == expected->current_player().index());
    REQUIRE(actual->players().size() == expected->players().size());
    for (auto i = 0; i < actual->players().size(); ++i)
        REQUIRE(actual->players().at(i) == expected->players().at(i));
}

} // namespace k10engine::Game
