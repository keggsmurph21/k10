#include "Common.h"

// NOLINTNEXTLINE(readability-function-size,google-readability-function-size)
TEST_CASE("Standard board first two rounds", "[Game] [Integration] [Board.Standard]")
{
    SECTION("Two players")
    {
        auto b = Board::Store::the().by_name(Board::Name::Standard);
        auto s = Scenario::Store::the().by_name(Scenario::Name::Standard);
        auto p = get_standard_parameters(2);
        auto g = Game::Game::initialize(b, s, p);

        bootstrap_tests();

        REQUIRE(g->players().size() == 2);

        gs.robber_location = 143;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;

        check_state();
        check_build_settlement(0, 54);
        check_no_actions(1);

        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(4)).type == ResType::Ok);
        settlements[4] = 0;
        settlements[13] = -1;
        settlements[14] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 2);
        check_no_actions(1);

        REQUIRE(g->execute_build_road(g->player(0), g->road(7)).type == ResType::Ok);
        roads[7] = 0;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        ps[1].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_to_root(1);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 51);

        REQUIRE(g->execute_build_settlement(g->player(1), g->junction(5)).type == ResType::Ok);
        settlements[5] = 1;
        settlements[15] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 1;
        ps[1].public_victory_points = 1;
        check_state();
        check_no_actions(0);
        check_build_road(1, 2);

        REQUIRE(g->execute_build_road(g->player(1), g->road(9)).type == ResType::Ok);
        roads[9] = 1;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        gs.round += 1;
        gs.is_first_round = false;
        gs.is_second_round = true;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].roads = 1;
        check_state();
        check_no_actions(0);
        check_to_root(1);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 49);

        REQUIRE(g->execute_build_settlement(g->player(1), g->junction(6)).type == ResType::Ok);
        settlements[6] = 1;
        settlements[16] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 2;
        ps[1].public_victory_points = 2;
        check_state();
        check_no_actions(0);
        check_build_road(1, 5);

        REQUIRE(g->execute_build_road(g->player(1), g->road(10)).type == ResType::Ok);
        roads[10] = 1;
        check_roads();

        gs.roads_built += 1;
        ps[1].vertex = Vertex::ChooseInitialResources;
        ps[1].roads = 2;
        check_state();
        check_no_actions(0);
        check_choose_initial_resources(1);

        REQUIRE(g->execute_choose_initial_resources(g->player(1), g->junction(5)).type == ResType::Ok);

        gs.turn += 1;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].is_current_player = false;
        ps[1].num_resources = 1;
        check_state();
        check_to_root(0);
        check_no_actions(1);

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);

        ps[0].vertex = Vertex::Root;
        check_state();
        check_build_settlement(0, 47);
        check_no_actions(1);

        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(26)).type == ResType::Ok);
        settlements[26] = 0;
        settlements[38] = -1;
        settlements[39] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 4);
        check_no_actions(1);

        REQUIRE(g->execute_build_road(g->player(0), g->road(18)).type == ResType::Ok);
        roads[18] = 0;
        check_roads();

        gs.roads_built += 1;
        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);
        check_no_actions(1);

        REQUIRE(g->execute_choose_initial_resources(g->player(0), g->junction(26)).type == ResType::Ok);

        gs.is_second_round = false;
        gs.turn += 1;
        gs.round += 1;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].num_resources = 2;
        check_state();
        check_to_root(0);
        check_no_actions(1);

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);
        check_no_actions(1);

        REQUIRE(g->execute_roll_dice(g->player(0), 6).type == ResType::Ok);

        gs.has_rolled = true;
        gs.dice_total = 6;
        check_state();
        check_end_turn(0);
        check_no_actions(1);

        REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

        gs.turn += 1;
        gs.has_rolled = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].is_current_player = false;
        ps[1].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_to_root(1);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_roll_dice(1);

        // dump_actions();
        delete g;
    }

    SECTION("Three players")
    {
        auto b = Board::Store::the().by_name(Board::Name::Standard);
        auto s = Scenario::Store::the().by_name(Scenario::Name::Standard);
        auto p = get_standard_parameters(3);
        auto g = Game::Game::initialize(b, s, p);

        bootstrap_tests();

        REQUIRE(g->players().size() == 3);

        gs.robber_location = 143;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[2].vertex = Vertex::WaitForTurn;

        check_state();
        check_build_settlement(0, 54);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(4)).type == ResType::Ok);
        settlements[4] = 0;
        settlements[13] = -1;
        settlements[14] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 2);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_build_road(g->player(0), g->road(7)).type == ResType::Ok);
        roads[7] = 0;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        ps[1].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_to_root(1);
        check_no_actions(2);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 51);
        check_no_actions(2);

        REQUIRE(g->execute_build_settlement(g->player(1), g->junction(5)).type == ResType::Ok);
        settlements[5] = 1;
        settlements[15] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 1;
        ps[1].public_victory_points = 1;
        check_state();
        check_no_actions(0);
        check_build_road(1, 2);
        check_no_actions(2);

        REQUIRE(g->execute_build_road(g->player(1), g->road(9)).type == ResType::Ok);
        roads[9] = 1;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].roads = 1;
        ps[1].is_current_player = false;
        ps[2].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_to_root(2);

        REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);

        ps[2].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_settlement(2, 49);

        REQUIRE(g->execute_build_settlement(g->player(2), g->junction(90)).type == ResType::Ok);
        settlements[90] = 2;
        settlements[71] = -1;
        settlements[105] = -1;
        settlements[106] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[2].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[2].settlements = 1;
        ps[2].public_victory_points = 1;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_road(2, 3);

        REQUIRE(g->execute_build_road(g->player(2), g->road(80)).type == ResType::Ok);
        roads[80] = 2;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        gs.round += 1;
        gs.is_first_round = false;
        gs.is_second_round = true;
        ps[2].vertex = Vertex::WaitForTurn;
        ps[2].roads = 1;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_to_root(2);

        REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);

        ps[2].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_settlement(2, 45);

        REQUIRE(g->execute_build_settlement(g->player(2), g->junction(91)).type == ResType::Ok);
        settlements[91] = 2;
        settlements[72] = -1;
        settlements[107] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[2].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[2].settlements = 2;
        ps[2].public_victory_points = 2;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_road(2, 7);

        REQUIRE(g->execute_build_road(g->player(2), g->road(63)).type == ResType::Ok);
        roads[63] = 2;
        check_roads();

        gs.roads_built += 1;
        ps[2].vertex = Vertex::ChooseInitialResources;
        ps[2].roads = 2;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_choose_initial_resources(2);

        REQUIRE(g->execute_choose_initial_resources(g->player(2), g->junction(91)).type == ResType::Ok);

        gs.turn += 1;
        ps[1].is_current_player = true;
        ps[2].is_current_player = false;
        ps[2].vertex = Vertex::WaitForTurn;
        ps[2].roads = 2;
        ps[2].num_resources = 3;
        check_state();
        check_no_actions(0);
        check_to_root(1);
        check_no_actions(2);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 42);
        check_no_actions(2);

        REQUIRE(g->execute_build_settlement(g->player(1), g->junction(6)).type == ResType::Ok);
        settlements[6] = 1;
        settlements[16] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 2;
        ps[1].public_victory_points = 2;
        check_state();
        check_no_actions(0);
        check_build_road(1, 5);
        check_no_actions(2);

        REQUIRE(g->execute_build_road(g->player(1), g->road(10)).type == ResType::Ok);
        roads[10] = 1;
        check_roads();

        gs.roads_built += 1;
        ps[1].vertex = Vertex::ChooseInitialResources;
        ps[1].roads = 2;
        check_state();
        check_no_actions(0);
        check_choose_initial_resources(1);
        check_no_actions(2);

        REQUIRE(g->execute_choose_initial_resources(g->player(1), g->junction(6)).type == ResType::Ok);

        gs.turn += 1;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].is_current_player = false;
        ps[1].num_resources = 1;
        check_state();
        check_to_root(0);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);

        ps[0].vertex = Vertex::Root;
        check_state();
        check_build_settlement(0, 40);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(26)).type == ResType::Ok);
        settlements[26] = 0;
        settlements[38] = -1;
        settlements[39] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 4);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_build_road(g->player(0), g->road(18)).type == ResType::Ok);
        roads[18] = 0;
        check_roads();

        gs.roads_built += 1;
        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_choose_initial_resources(g->player(0), g->junction(26)).type == ResType::Ok);

        gs.is_second_round = false;
        gs.turn += 1;
        gs.round += 1;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].num_resources = 2;
        check_state();
        check_to_root(0);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_roll_dice(g->player(0), 6).type == ResType::Ok);

        gs.has_rolled = true;
        gs.dice_total = 6;
        ps[2].num_resources += 1;
        check_state();
        check_end_turn(0);
        check_no_actions(1);
        check_no_actions(2);

        // dump_actions();
        delete g;
    }
}

// NOLINTNEXTLINE(readability-function-size,google-readability-function-size)
TEST_CASE("Standard board", "[Game] [Integration] [Board.Standard]")
{
    SECTION("Three players immediately ending turns")
    {
        auto b = Board::Store::the().by_name(Board::Name::Standard);
        auto s = Scenario::Store::the().by_name(Scenario::Name::Standard);
        auto p = get_standard_parameters(3);
        auto g = Game::Game::initialize(b, s, p);

        bootstrap_tests();

        do_first_two_rounds_standard_3p();

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);
        check_no_actions(1);
        check_no_actions(1);

        REQUIRE(g->execute_roll_dice(g->player(0), 10).type == ResType::Ok);

        gs.has_rolled = true;
        gs.dice_total = 10;
        ps[0].vertex = Vertex::Root;
        ps[2].num_resources += 2;
        check_state();
        check_end_turn(0);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

        gs.turn += 1;
        gs.has_rolled = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].is_current_player = false;
        ps[1].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_to_root(1);
        check_no_actions(2);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_roll_dice(1);
        check_no_actions(2);

        REQUIRE(g->execute_roll_dice(g->player(1), 8).type == ResType::Ok);

        gs.has_rolled = true;
        gs.dice_total = 8;
        ps[2].num_resources += 3;
        check_state();
        check_no_actions(0);
        check_end_turn(1);
        check_no_actions(2);

        REQUIRE(g->execute_end_turn(g->player(1)).type == ResType::Ok);

        gs.turn += 1;
        gs.has_rolled = false;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].is_current_player = false;
        ps[2].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_to_root(2);

        REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);

        ps[2].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_roll_dice(2);

        REQUIRE(g->execute_roll_dice(g->player(2), 5).type == ResType::Ok);

        gs.has_rolled = true;
        gs.dice_total = 5;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_end_turn(2);

        REQUIRE(g->execute_end_turn(g->player(2)).type == ResType::Ok);

        gs.turn += 1;
        gs.round += 1;
        gs.has_rolled = false;
        ps[0].is_current_player = true;
        ps[2].vertex = Vertex::WaitForTurn;
        ps[2].is_current_player = false;
        check_state();
        check_to_root(0);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_roll_dice(g->player(0), 9).type == ResType::Ok);

        gs.has_rolled = true;
        gs.dice_total = 9;
        check_state();
        check_end_turn(0);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

        gs.turn += 1;
        gs.has_rolled = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].is_current_player = false;
        ps[1].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_to_root(1);
        check_no_actions(2);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_roll_dice(1);
        check_no_actions(2);

        REQUIRE(g->execute_roll_dice(g->player(1), 4).type == ResType::Ok);

        gs.has_rolled = true;
        gs.dice_total = 4;
        ps[0].num_resources += 1;
        check_state();
        check_no_actions(0);
        check_end_turn(1);
        check_no_actions(2);

        REQUIRE(g->execute_end_turn(g->player(1)).type == ResType::Ok);

        gs.turn += 1;
        gs.has_rolled = false;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].is_current_player = false;
        ps[2].is_current_player = true;

        REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);

        ps[2].vertex = Vertex::Root;

        REQUIRE(g->execute_roll_dice(g->player(2), 10).type == ResType::Ok);

        gs.has_rolled = true;
        gs.dice_total = 10;
        ps[2].num_resources += 2;
        check_state();

        // dump_actions();
        delete g;
    }

    SECTION("Three players offering trades")
    {
        auto b = Board::Store::the().by_name(Board::Name::Standard);
        auto s = Scenario::Store::the().by_name(Scenario::Name::Standard);
        auto p = get_standard_parameters(3);
        auto g = Game::Game::initialize(b, s, p);

        bootstrap_tests();

        do_first_two_rounds_standard_3p();

        /*
        exec_error(0,
                   trade({ 1 }, { { Resource::Brick, 2 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(0,
                   trade({ 1 }, { { Resource::Ore, 2 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(0,
                   trade({ 1 }, { { Resource::Sheep, 1 } }, { { Resource::Brick, 1 } }),
                   ResType::CannotAfford);
        exec_error(0,
                   trade({ 1 }, { { Resource::Wheat, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(0,
                   trade({ 1 }, { { Resource::Wood, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        // exec_error(0,trade({ 1}, { { Resource::Brick, 1 } }, { { Resource::Sheep, 1 } }),
        // ResType::CannotAfford); exec_error(0,trade({ 1}, { { Resource::Ore, 1 } }, { {
        // Resource::Sheep, 1 } }), ResType::CannotAfford);

        // ...

        exec_error(1,
                   trade({ 0 }, { { Resource::Brick, 2 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(1,
                   trade({ 0 }, { { Resource::Ore, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(1,
                   trade({ 0 }, { { Resource::Sheep, 1 } }, { { Resource::Brick, 1 } }),
                   ResType::CannotAfford);
        exec_error(1,
                   trade({ 0 }, { { Resource::Wheat, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(1,
                   trade({ 0 }, { { Resource::Wood, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        // exec_error(1,trade({ 0}, { { Resource::Brick, 1 } }, { { Resource::Sheep, 1 } }),
        // ResType::CannotAfford);

        // ...

        exec_error(2,
                   trade({ 0 }, { { Resource::Brick, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(2,
                   trade({ 0 }, { { Resource::Ore, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(2,
                   trade({ 0 }, { { Resource::Sheep, 2 } }, { { Resource::Brick, 1 } }),
                   ResType::CannotAfford);
        exec_error(2,
                   trade({ 0 }, { { Resource::Wheat, 5 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(2,
                   trade({ 0 }, { { Resource::Wood, 5 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        // exec_error(2,trade({ 0}, { { Resource::Sheep, 1 } }, { { Resource::Brick, 1 } }),
        // ResType::CannotAfford); exec_error(2,trade({ 0}, { { Resource::Wheat, 4 } }, { {
        // Resource::Sheep, 1 } }), ResType::CannotAfford); exec_error(2,trade({ 0}, { {
        // Resource::Wood, 4 } }, { { Resource::Sheep, 1 } }), ResType::CannotAfford);
        */

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(0), 10).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(1), 10).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(1)).type == ResType::Ok);

        REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(2), 10).type == ResType::Ok);

        gs.dice_total = 10;
        gs.has_rolled = true;
        gs.turn += 2;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[2].is_current_player = true;
        ps[2].num_resources += 6;
        ps[2].vertex = Vertex::Root;

        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_end_turn(2);

        /*
         * p0 has { brick: 1, ore: 1 }
         * p1 has { brick: 1 }
         * p2 has { sheep: 1, wood: 4, wheat: 4 }
         */

        REQUIRE(g->execute_offer_trade(g->player(2),
                                       { &g->player(2),
                                         { &g->player(0), &g->player(1) },
                                         { { Resource::Wheat, 1 } },
                                         { { Resource::Brick, 1 } } })
                    .type
                == ResType::Ok);

        gs.has_current_trade = true;
        gs.should_wait_for_trade = true;
        gs.num_trades_offered_this_turn += 1;
        ps[0].can_accept_trade = true;
        ps[1].can_accept_trade = true;
        ps[2].vertex = Vertex::WaitForTradeResponses;
        check_state();

        REQUIRE(g->execute_cancel_trade(g->player(2)).type == ResType::Ok);

        gs.has_current_trade = false;
        gs.should_wait_for_trade = false;
        ps[0].can_accept_trade = false;
        ps[1].can_accept_trade = false;
        ps[2].vertex = Vertex::Root;
        check_state();

        REQUIRE(g->execute_offer_trade(g->player(2),
                                       { &g->player(2),
                                         { &g->player(0), &g->player(1) },
                                         { { Resource::Wheat, 1 } },
                                         { { Resource::Brick, 1 } } })
                    .type
                == ResType::Ok);

        gs.has_current_trade = true;
        gs.should_wait_for_trade = true;
        gs.num_trades_offered_this_turn += 1;
        ps[0].can_accept_trade = true;
        ps[1].can_accept_trade = true;
        ps[2].vertex = Vertex::WaitForTradeResponses;

        REQUIRE(g->execute_decline_trade(g->player(0)).type == ResType::Ok);

        ps[0].has_declined_trade = true;
        check_state();

        REQUIRE(g->execute_decline_trade(g->player(1)).type == ResType::Ok);

        gs.has_current_trade = false;
        gs.should_wait_for_trade = false;
        ps[0].can_accept_trade = false;
        ps[0].has_declined_trade = false;
        ps[1].can_accept_trade = false;
        ps[1].has_declined_trade = false;
        ps[2].vertex = Vertex::Root;
        check_state();

        REQUIRE(g->execute_offer_trade(g->player(2),
                                       { &g->player(2),
                                         { &g->player(0), &g->player(1) },
                                         { { Resource::Wheat, 1 } },
                                         { { Resource::Brick, 1 } } })
                    .type
                == ResType::Ok);

        gs.has_current_trade = true;
        gs.should_wait_for_trade = true;
        gs.num_trades_offered_this_turn += 1;
        ps[0].can_accept_trade = true;
        ps[1].can_accept_trade = true;
        ps[2].vertex = Vertex::WaitForTradeResponses;

        REQUIRE(g->execute_accept_trade(g->player(0)).type == ResType::Ok);

        gs.has_current_trade = false;
        gs.should_wait_for_trade = false;
        ps[0].can_accept_trade = false;
        ps[0].has_declined_trade = false;
        ps[1].can_accept_trade = false;
        ps[1].has_declined_trade = false;
        ps[2].vertex = Vertex::Root;
        check_state();

        REQUIRE(g->execute_offer_trade(g->player(2),
                                       { &g->player(2),
                                         { &g->player(0), &g->player(1) },
                                         { { Resource::Wheat, 1 } },
                                         { { Resource::Brick, 1 } } })
                    .type
                == ResType::Ok);
        REQUIRE(g->execute_decline_trade(g->player(1)).type == ResType::Ok);

        gs.num_trades_offered_this_turn += 1;
        check_state();

        REQUIRE(g->execute_offer_trade(g->player(2),
                                       { &g->player(2),
                                         { &g->player(0), &g->player(1) },
                                         { { Resource::Wheat, 1 } },
                                         { { Resource::Wood, 1 } } })
                    .type
                == ResType::Ok);

        gs.num_trades_offered_this_turn += 1;
        check_state();

        REQUIRE(g->execute_offer_trade(
                     g->player(2),
                     { &g->player(2), { &g->player(0) }, { { Resource::Wheat, 1 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::Ok);

        gs.num_trades_offered_this_turn += 1;
        check_state();

        // dump_actions();
        delete g;
    }

    SECTION("Three players building roads")
    {
        auto b = Board::Store::the().by_name(Board::Name::Standard);
        auto s = Scenario::Store::the().by_name(Scenario::Name::Standard);
        auto p = get_standard_parameters(3);
        auto g = Game::Game::initialize(b, s, p);

        bootstrap_tests();

        do_first_two_rounds_standard_3p();

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(0), 10).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(1), 10).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(1)).type == ResType::Ok);

        REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(2), 10).type == ResType::Ok);

        REQUIRE(g->execute_offer_trade(
                     g->player(2),
                     { &g->player(2), { &g->player(0) }, { { Resource::Wheat, 1 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::Ok);
        REQUIRE(g->execute_accept_trade(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_offer_trade(
                     g->player(2),
                     { &g->player(2), { &g->player(1) }, { { Resource::Wheat, 1 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::Ok);
        REQUIRE(g->execute_accept_trade(g->player(1)).type == ResType::Ok);

        gs.dice_total = 10;
        gs.has_rolled = true;
        gs.turn += 2;
        gs.num_trades_offered_this_turn = 2;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[2].is_current_player = true;
        ps[2].num_resources += 6;
        ps[2].vertex = Vertex::Root;

        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_end_turn(2);

        /*
         * p0 has { ore: 1, wheat: 1 }
         * p1 has { wheat: 1 }
         * p2 has { brick: 2, sheep: 1, wood: 4, wheat: 2 }
         */

        check_build_road(2, 8);

        REQUIRE(g->execute_build_road(g->player(2), g->road(48)).type == ResType::Ok);
        roads[48] = 2;
        check_roads();

        gs.roads_built += 1;
        ps[2].roads += 1;
        ps[2].num_resources -= 2;
        check_state();

        check_build_road(2, 9);

        REQUIRE(g->execute_build_road(g->player(2), g->road(32)).type == ResType::InvalidNodeId);
        REQUIRE(g->execute_build_road(g->player(2), g->road(33)).type == ResType::Ok);
        roads[33] = 2;
        check_roads();

        gs.roads_built += 1;
        ps[2].roads += 1;
        ps[2].num_resources -= 2;
        check_state();

        // dump_actions();
        delete g;
    }

    SECTION("Three players building settlements")
    {
        auto b = Board::Store::the().by_name(Board::Name::Standard);
        auto s = Scenario::Store::the().by_name(Scenario::Name::Standard);
        auto p = get_standard_parameters(3);
        auto g = Game::Game::initialize(b, s, p);

        bootstrap_tests();

        do_first_two_rounds_standard_3p();

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(0), 10).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(1), 10).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(1)).type == ResType::Ok);

        REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(2), 10).type == ResType::Ok);

        REQUIRE(g->execute_offer_trade(
                     g->player(2),
                     { &g->player(2), { &g->player(0) }, { { Resource::Wheat, 1 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::Ok);
        REQUIRE(g->execute_accept_trade(g->player(0)).type == ResType::Ok);

        gs.dice_total = 10;
        gs.has_rolled = true;
        gs.turn += 2;
        gs.num_trades_offered_this_turn = 1;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[2].is_current_player = true;
        ps[2].num_resources += 6;
        ps[2].vertex = Vertex::Root;

        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_end_turn(2);

        /*
         * p0 has { ore: 1, wheat: 1 }
         * p1 has { brick: 1 }
         * p2 has { brick: 1, sheep: 1, wood: 4, wheat: 3 }
         */

        check_build_settlement(2, 1);

        REQUIRE(g->execute_build_settlement(g->player(2), g->junction(56)).type == ResType::Ok);
        settlements[56] = 2;
        settlements[40] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[2].settlements += 1;
        ps[2].num_resources -= 4;
        ps[2].public_victory_points += 1;
        check_state();

        // dump_actions();
        delete g;
    }

    SECTION("Three players buying development cards")
    {
        auto b = Board::Store::the().by_name(Board::Name::Standard);
        auto s = Scenario::Store::the().by_name(Scenario::Name::Standard);
        auto p = get_standard_parameters(3);
        auto g = Game::Game::initialize(b, s, p);

        bootstrap_tests();

        do_first_two_rounds_standard_3p();

        const size_t rounds = 25;

        for (size_t i = 0; i < rounds; ++i) {
            REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
            REQUIRE(g->execute_roll_dice(g->player(0), 4).type == ResType::Ok);
            REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

            REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);
            REQUIRE(g->execute_roll_dice(g->player(1), 8).type == ResType::Ok);
            REQUIRE(g->execute_end_turn(g->player(1)).type == ResType::Ok);

            REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);
            REQUIRE(g->execute_roll_dice(g->player(2), 8).type == ResType::Ok);
            REQUIRE(g->execute_end_turn(g->player(2)).type == ResType::Ok);
        }

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(0), 4).type == ResType::Ok);

        /*
         * p0 has { brick: 1, ore: 2 + rounds }
         * p1 has { brick: 1 }
         * p2 has { sheep: 1 + 2 * rounds, wheat: 1 + rounds, wood: 1 }
         */

        REQUIRE(g->execute_offer_trade(g->player(0),
                                       { &g->player(0),
                                         { &g->player(2) },
                                         { { Resource::Brick, 1 } },
                                         { { Resource::Sheep, 1 + 4 * rounds }, { Resource::Wheat, 1 + 2 * rounds } } })
                    .type
                == ResType::Ok);
        REQUIRE(g->execute_accept_trade(g->player(2)).type == ResType::Ok);

        gs.dice_total = 4;
        gs.has_rolled = true;
        gs.turn += rounds * 3;
        gs.round += rounds;
        gs.num_trades_offered_this_turn = 1;
        ps[0].vertex = Vertex::Root;
        ps[0].num_resources += 2 + rounds * 7;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[2].vertex = Vertex::WaitForTurn;
        ps[2].num_resources = 2;

        check_state();
        check_end_turn(0);
        check_no_actions(1);
        check_no_actions(2);

        for (size_t i = 0; i < rounds; ++i) {
            REQUIRE(g->execute_build_development_card(g->player(0)).type == ResType::Ok);
        }

        // no more development cards
        REQUIRE(g->execute_build_development_card(g->player(0)).type == ResType::InvalidEdgeChoice);

        gs.development_cards_built += rounds;
        ps[0].num_resources -= rounds * 3;
        ps[0].num_unplayed_development_cards += rounds;
        check_state();

        REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(1), 12).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(1)).type == ResType::Ok);

        REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(2), 12).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(2)).type == ResType::Ok);

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(0), 12).type == ResType::Ok);

        gs.dice_total = 12;
        gs.turn += 3;
        gs.round += 1;
        gs.num_trades_offered_this_turn = 0;
        check_state();

        REQUIRE(g->execute_play_knight(g->player(0), g->hex(143)).type == ResType::InvalidNodeId);
        REQUIRE(g->execute_play_knight(g->player(0), g->hex(141)).type == ResType::Ok);

        gs.robber_location = 141;
        ps[0].num_played_development_cards += 1;
        ps[0].num_unplayed_development_cards -= 1;
        ps[0].vertex = Vertex::AfterMovingRobber;
        ps[0].army_size += 1;
        check_state();

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_play_knight(g->player(0), g->hex(19)).type == ResType::Ok);
        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_play_knight(g->player(0), g->hex(21)).type == ResType::Ok);

        gs.robber_location = 21;
        gs.has_largest_army = 0;
        gs.largest_army = 3;
        gs.can_steal = true;
        ps[0].num_played_development_cards += 2;
        ps[0].num_unplayed_development_cards -= 2;
        ps[0].army_size += 2;
        ps[0].public_victory_points += 2;
        check_state();

        REQUIRE(g->execute_steal(g->player(0), g->player(0)).type == ResType::InvalidPlayerId);
        REQUIRE(g->execute_steal(g->player(0), g->player(2)).type == ResType::InvalidPlayerId);
        REQUIRE(g->execute_steal(g->player(0), g->player(1)).type == ResType::Ok);

        gs.can_steal = false;
        ps[0].num_resources += 1;
        ps[1].num_resources -= 1;
        check_state();

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_play_monopoly(g->player(0), Resource::Ore).type == ResType::Ok);
        REQUIRE(g->execute_play_monopoly(g->player(0), Resource::Wood).type == ResType::Ok);

        ps[0].vertex = State::Vertex::Root;
        ps[0].num_played_development_cards += 2;
        ps[0].num_unplayed_development_cards -= 2;
        ps[0].num_resources += 1;
        ps[2].num_resources -= 1;
        check_state();

        REQUIRE(g->execute_play_road_building(g->player(0), g->road(8), g->road(22)).type == ResType::InvalidNodeId);

        REQUIRE(g->execute_play_road_building(g->player(0), g->road(8), g->road(20)).type == ResType::Ok);

        roads[8] = 0;
        roads[20] = 0;
        check_roads();

        gs.roads_built += 2;
        ps[0].roads += 2;
        ps[0].num_played_development_cards += 1;
        ps[0].num_unplayed_development_cards -= 1;
        check_state();

        for (size_t i = 0; i < 5; ++i) {
            REQUIRE(g->execute_play_victory_point(g->player(0)).type == ResType::Ok);
        }

        ps[0].public_victory_points += 5;
        ps[0].num_played_development_cards += 5;
        ps[0].num_unplayed_development_cards -= 5;
        check_state();

        REQUIRE(g->execute_play_year_of_plenty(g->player(0), Resource::Wood, Resource::Wood).type == ResType::Ok);

        ps[0].num_resources += 1;
        ps[0].num_played_development_cards += 1;
        ps[0].num_unplayed_development_cards -= 1;
        check_state();

        // dump_actions();
        delete g;
    }

    SECTION("Three players trading with bank")
    {
        auto b = Board::Store::the().by_name(Board::Name::Standard);
        auto s = Scenario::Store::the().by_name(Scenario::Name::Standard);
        auto p = get_standard_parameters(3);
        auto g = Game::Game::initialize(b, s, p);

        bootstrap_tests();

        do_first_two_rounds_standard_3p();

        const std::vector<size_t> rolls = { 2, 4, 8, 10 };
        const size_t rounds_per_roll = 12;

        for (const auto& roll : rolls) {
            for (size_t i = 0; i < rounds_per_roll; ++i) {
                REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
                REQUIRE(g->execute_roll_dice(g->player(0), roll).type == ResType::Ok);
                REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

                REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);
                REQUIRE(g->execute_roll_dice(g->player(1), roll).type == ResType::Ok);
                REQUIRE(g->execute_end_turn(g->player(1)).type == ResType::Ok);

                REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);
                REQUIRE(g->execute_roll_dice(g->player(2), roll).type == ResType::Ok);
                REQUIRE(g->execute_end_turn(g->player(2)).type == ResType::Ok);
            }
        }

        gs.dice_total = 10;
        gs.turn += 3 * rolls.size() * rounds_per_roll;
        gs.round += rolls.size() * rounds_per_roll;
        ps[0].is_current_player = true;
        ps[0].num_resources += 3 * 3 * rounds_per_roll;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[2].num_resources += 3 * 5 * rounds_per_roll;
        check_state();

        /*
         * p0 has { brick: 73, ore: 37 }
         * p1 has { brick: 1 }
         * p2 has { sheep: 73, wood: 37, wheat: 73 }
         */

        // Even things up so that each player has at least ten of each resource type

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(0), 12).type == ResType::Ok);
        REQUIRE(g->execute_offer_trade(g->player(0),
                                       { &g->player(0),
                                         { &g->player(1) },
                                         { { Resource::Brick, 10 }, { Resource::Ore, 20 } },
                                         { { Resource::Brick, 1 } } })
                    .type
                == ResType::Ok);
        REQUIRE(g->execute_accept_trade(g->player(1)).type == ResType::Ok);
        REQUIRE(g->execute_offer_trade(g->player(0),
                                       { &g->player(0),
                                         { &g->player(2) },
                                         { { Resource::Brick, 10 }, { Resource::Ore, 10 } },
                                         { { Resource::Sheep, 10 }, { Resource::Wheat, 10 }, { Resource::Wood, 10 } } })
                    .type
                == ResType::Ok);
        REQUIRE(g->execute_accept_trade(g->player(2)).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(1), 12).type == ResType::Ok);
        REQUIRE(g->execute_offer_trade(g->player(1),
                                       { &g->player(1),
                                         { &g->player(2) },
                                         { { Resource::Brick, 1 } },
                                         { { Resource::Sheep, 10 }, { Resource::Wheat, 10 }, { Resource::Wood, 10 } } })
                    .type
                == ResType::Ok);
        REQUIRE(g->execute_accept_trade(g->player(2)).type == ResType::Ok);

        // p1 can trade { wheat @ 2::1, <everything else> @ 4::1 }

        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Brick, 3 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Brick, 5 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Brick, 8 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Brick, 4 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Ore, 3 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Ore, 5 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Ore, 8 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Ore, 4 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Sheep, 3 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Sheep, 5 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Sheep, 8 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Sheep, 4 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Wheat, 1 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Wheat, 3 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Wheat, 4 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Wheat, 2 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Wood, 3 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Wood, 5 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Wood, 8 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1), {}, { { Resource::Wood, 4 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_trade_with_bank(
                     g->player(1),
                     { &g->player(1), {}, { { Resource::Brick, 4 }, { Resource::Ore, 4 } }, { { Resource::Wood, 2 } } })
                    .type
                == ResType::Ok);
        REQUIRE(g->execute_trade_with_bank(g->player(1),
                                           { &g->player(1),
                                             {},
                                             { { Resource::Wheat, 6 }, { Resource::Ore, 4 }, { Resource::Wood, 4 } },
                                             { { Resource::Sheep, 5 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_end_turn(g->player(1)).type == ResType::Ok);

        REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(2), 12).type == ResType::Ok);

        // p2 can trade { <everything> @ 4::1 }

        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Brick, 3 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Brick, 5 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Brick, 8 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Brick, 4 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Ore, 3 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Ore, 5 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Ore, 8 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Ore, 4 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Sheep, 3 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Sheep, 5 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Sheep, 8 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Sheep, 4 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Wheat, 3 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Wheat, 5 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Wheat, 8 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Wheat, 4 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Wood, 3 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Wood, 5 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Wood, 8 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(2),
                                           { &g->player(2), {}, { { Resource::Wood, 4 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_end_turn(g->player(2)).type == ResType::Ok);

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(0), 12).type == ResType::Ok);

        // p3 can trade { <everything> @ 3::1 }

        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Brick, 2 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Brick, 4 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Brick, 6 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Brick, 3 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Ore, 2 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Ore, 4 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Ore, 6 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Ore, 3 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Sheep, 2 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Sheep, 4 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Sheep, 6 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Sheep, 3 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Wheat, 2 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Wheat, 4 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Wheat, 6 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Wheat, 3 } }, { { Resource::Wood, 1 } } })
                    .type
                == ResType::Ok);

        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Wood, 2 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Wood, 4 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::InvalidTrade);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Wood, 6 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::StopFlexing);
        REQUIRE(g->execute_trade_with_bank(g->player(0),
                                           { &g->player(0), {}, { { Resource::Wood, 3 } }, { { Resource::Brick, 1 } } })
                    .type
                == ResType::Ok);

        // dump_actions();
        delete g;
    }

    SECTION("Three players trading with bank")
    {
        auto b = Board::Store::the().by_name(Board::Name::Standard);
        auto s = Scenario::Store::the().by_name(Scenario::Name::Standard);
        auto p = get_standard_parameters(3);
        auto g = Game::Game::initialize(b, s, p);

        bootstrap_tests();

        do_first_two_rounds_standard_3p();

        const std::vector<size_t> rolls = { 2, 4, 8, 10 };
        const size_t rounds_per_roll = 12;

        for (const auto& roll : rolls) {
            for (size_t i = 0; i < rounds_per_roll; ++i) {
                REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
                REQUIRE(g->execute_roll_dice(g->player(0), roll).type == ResType::Ok);
                REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

                REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);
                REQUIRE(g->execute_roll_dice(g->player(1), roll).type == ResType::Ok);
                REQUIRE(g->execute_end_turn(g->player(1)).type == ResType::Ok);

                REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);
                REQUIRE(g->execute_roll_dice(g->player(2), roll).type == ResType::Ok);
                REQUIRE(g->execute_end_turn(g->player(2)).type == ResType::Ok);
            }
        }

        gs.dice_total = 10;
        gs.turn += 3 * rolls.size() * rounds_per_roll;
        gs.round += rolls.size() * rounds_per_roll;
        ps[0].is_current_player = true;
        ps[0].num_resources += 3 * 3 * rounds_per_roll;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[2].num_resources += 3 * 5 * rounds_per_roll;
        check_state();

        /*
         * p0 has { brick: 73, ore: 37 }
         * p1 has { brick: 1 }
         * p2 has { sheep: 73, wood: 37,
         * wheat: 73 }
         */

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(0), 12).type == ResType::Ok);
        REQUIRE(g->execute_trade_with_bank(
                     g->player(0), { &g->player(0), {}, { { Resource::Brick, 6 } }, { { Resource::Wheat, 2 } } })
                    .type
                == ResType::Ok);
        REQUIRE(g->execute_build_city(g->player(0), g->junction(4)).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

        gs.dice_total = 12;
        gs.turn += 1;
        gs.cities_built += 1;
        ps[0].is_current_player = false;
        ps[0].num_resources -= 9;
        ps[0].public_victory_points += 1;
        ps[0].cities += 1;
        ps[1].is_current_player = true;
        check_state();

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(1), 2).type == ResType::Ok);

        gs.has_rolled = true;
        gs.dice_total = 2;
        ps[0].num_resources += 3;
        ps[1].vertex = Vertex::Root;
        check_state();

        // dump_actions();
        delete g;
    }

    SECTION("Three players rolling sevens")
    {
        auto b = Board::Store::the().by_name(Board::Name::Standard);
        auto s = Scenario::Store::the().by_name(Scenario::Name::Standard);
        auto p = get_standard_parameters(3);
        auto g = Game::Game::initialize(b, s, p);

        bootstrap_tests();

        do_first_two_rounds_standard_3p();

        /*
         * p0 has { brick: 1, ore: 1
         * } p1 has { brick: 1 } p2
         * has { sheep: 1, wood: 1,
         * wheat: 1 }
         */

        // NB: Can only roll sevens
        // on third round by
        // explicitly
        // ArgType::DiceRoll'ing

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(0), 7).type == ResType::Ok);

        REQUIRE(g->execute_move_robber(g->player(0), g->hex(143)).type == ResType::InvalidNodeId);
        REQUIRE(g->execute_move_robber(g->player(0), g->hex(141)).type == ResType::Ok);
        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

        gs.is_roll_seven = true;
        gs.robber_location = 141;
        gs.dice_total = 7;
        gs.turn += 1;
        ps[0].is_current_player = false;
        ps[1].is_current_player = true;
        check_state();

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(1), 7).type == ResType::Ok);
        REQUIRE(g->execute_move_robber(g->player(1), g->hex(21)).type == ResType::Ok);
        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(1)).type == ResType::Ok);

        gs.turn += 1;
        gs.robber_location = 21;
        ps[1].is_current_player = false;
        ps[2].is_current_player = true;
        check_state();

        REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(2), 7).type == ResType::Ok);
        REQUIRE(g->execute_move_robber(g->player(2), g->hex(19)).type == ResType::Ok);
        REQUIRE(g->execute_steal(g->player(2), g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(2)).type == ResType::Ok);

        gs.turn += 1;
        gs.round += 1;
        gs.robber_location = 19;
        ps[0].is_current_player = true;
        ps[0].num_resources -= 1;
        ps[2].is_current_player = false;
        ps[2].num_resources += 1;
        check_state();

        const std::vector<size_t> rolls = { 2, 4, 8, 10 };

        for (const auto& roll : rolls) {
            REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
            REQUIRE(g->execute_roll_dice(g->player(0), roll).type == ResType::Ok);
            REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

            REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);
            REQUIRE(g->execute_roll_dice(g->player(1), roll).type == ResType::Ok);
            REQUIRE(g->execute_end_turn(g->player(1)).type == ResType::Ok);

            REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);
            REQUIRE(g->execute_roll_dice(g->player(2), roll).type == ResType::Ok);
            REQUIRE(g->execute_end_turn(g->player(2)).type == ResType::Ok);
        }

        gs.is_roll_seven = false;
        gs.dice_total = 10;
        gs.turn += 3 * rolls.size();
        gs.round += rolls.size();
        ps[0].is_current_player = true;
        ps[0].num_resources = 10;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[2].num_resources = 19;
        check_state();

        /*
         * p0 has { brick: 7 or 8,
         * ore: 2 or 3 } p1 has {
         * brick: 1 } p2 has {
         * brick: 0 or 1, ore: 0 or
         * 1, sheep: 7, wood: 4,
         * wheat: 7 }
         */

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(0), 7).type == ResType::Ok);

        gs.has_rolled = true;
        gs.is_roll_seven = true;
        gs.should_wait_for_discard = true;
        gs.dice_total = 7;
        ps[0].vertex = Vertex::AfterRollingSeven;
        ps[0].num_to_discard = 5;
        ps[2].num_to_discard = 9;
        check_state();

        REQUIRE(g->execute_discard(g->player(0), { { Resource::Brick, 6 } }).type == ResType::StopFlexing);
        REQUIRE(g->execute_discard(g->player(2), { { Resource::Sheep, 5 }, { Resource::Wheat, 5 } }).type
                == ResType::StopFlexing);
        REQUIRE(g->execute_discard(g->player(2), { { Resource::Sheep, 9 } }).type == ResType::CannotAfford);
        REQUIRE(g->execute_discard(g->player(2), { { Resource::Sheep, 5 } }).type == ResType::Ok);

        ps[2].num_to_discard = 4;
        ps[2].num_resources -= 5;
        check_state();

        REQUIRE(g->execute_discard(g->player(0), { { Resource::Brick, 4 } }).type == ResType::Ok);

        ps[0].num_to_discard = 1;
        ps[0].num_resources -= 4;
        check_state();

        REQUIRE(g->execute_discard(g->player(2), { { Resource::Wheat, 4 } }).type == ResType::Ok);

        ps[2].num_to_discard = 0;
        ps[2].num_resources = 10;
        check_state();
        check_no_actions(2);

        REQUIRE(g->execute_discard(g->player(0), { { Resource::Brick, 1 } }).type == ResType::Ok);

        gs.should_wait_for_discard = false;
        ps[0].num_to_discard = 0;
        ps[0].num_resources = 5;
        check_state();

        REQUIRE(g->execute_move_robber(g->player(0), g->hex(141)).type == ResType::Ok);
        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);
        REQUIRE(g->execute_end_turn(g->player(0)).type == ResType::Ok);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);
        REQUIRE(g->execute_roll_dice(g->player(1), 7).type == ResType::Ok);

        gs.should_wait_for_discard = true;
        gs.turn += 1;
        gs.robber_location = 141;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[1].is_current_player = true;
        ps[1].vertex = Vertex::AfterRollingSeven;
        ps[2].num_to_discard = 5;
        check_state();

        REQUIRE(g->execute_discard(g->player(2), { { Resource::Wood, 4 } }).type == ResType::Ok);

        ps[2].num_to_discard = 1;
        ps[2].num_resources = 6;
        check_state();

        REQUIRE(g->execute_discard(g->player(2), { { Resource::Wheat, 2 } }).type == ResType::StopFlexing);
        REQUIRE(g->execute_discard(g->player(2), { { Resource::Wheat, 1 } }).type == ResType::Ok);

        gs.should_wait_for_discard = false;
        ps[2].num_to_discard = 0;
        ps[2].num_resources = 5;
        check_state();
        check_no_actions(2);

        REQUIRE(g->execute_move_robber(g->player(1), g->hex(143)).type == ResType::Ok);

        // dump_actions();
        delete g;
    }
}
