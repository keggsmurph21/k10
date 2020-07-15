#include "Common.h"

// NOLINTNEXTLINE(readability-function-size,google-readability-function-size)
TEST_CASE("Single board first two rounds", "[Game] [Integration] [Board.Single]")
{
    SECTION("One player")
    {
        auto b = Board::Store::the().by_name(Board::Name::Single);
        auto s = Scenario::get_single_scenario();
        auto p = get_single_parameters();
        auto g = Game::Game::initialize(b, *s, p);

        bootstrap_tests();

        REQUIRE(g->players().size() == 1);

        gs.robber_location = 9;
        ps[0].is_current_player = true;

        check_state();
        check_build_settlement(0, 6);

        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(2)).type == ResType::Ok);
        settlements[2] = 0;
        settlements[5] = -1;
        settlements[6] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 2);

        REQUIRE(g->execute_build_road(g->player(0), g->road(4)).type == ResType::Ok);
        roads[4] = 0;
        check_roads();

        gs.roads_built += 1;
        gs.is_first_round = false;
        gs.is_second_round = true;
        gs.round += 1;
        gs.turn += 1;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        check_state();
        check_to_root(0);

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);

        ps[0].vertex = Vertex::Root;
        check_state();
        check_build_settlement(0, 3);

        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(12)).type == ResType::Ok);
        settlements[12] = 0;
        settlements[16] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 4);

        REQUIRE(g->execute_build_road(g->player(0), g->road(10)).type == ResType::Ok);
        roads[10] = 0;
        check_roads();

        gs.roads_built += 1;
        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);

        REQUIRE(g->execute_choose_initial_resources(g->player(0), g->junction(12)).type == ResType::Ok);

        gs.is_second_round = false;
        gs.round += 1;
        gs.turn += 1;
        ps[0].vertex = Vertex::WaitForTurn;
        check_state();
        check_to_root(0);

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);

        REQUIRE(g->execute_roll_dice(g->player(0), 12).type == ResType::Ok);

        gs.has_rolled = true;
        gs.dice_total = 12;
        check_state();
        check_end_turn(0);

        // dump_actions();
        delete g;
    }
}
