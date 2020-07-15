#include "Common.h"

// NOLINTNEXTLINE(readability-function-size,google-readability-function-size)
TEST_CASE("Triple board first two rounds", "[Game] [Integration] [Board.Triple]")
{
    SECTION("One player")
    {
        auto b = Board::Store::the().by_name(Board::Name::Triple);
        auto s = Scenario::Store::the().by_name(Scenario::Name::Triple);
        auto p = get_triple_parameters(1);
        auto g = Game::Game::initialize(b, s, p);

        bootstrap_tests();

        REQUIRE(g->players().size() == 1);

        gs.robber_location = 30;
        ps[0].is_current_player = true;

        check_state();
        check_build_settlement(0, 13);

        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(20)).type == ResType::Ok);
        settlements[20] = 0;
        settlements[10] = -1;
        settlements[26] = -1;
        settlements[27] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 3);

        REQUIRE(g->execute_build_road(g->player(0), g->road(15)).type == ResType::Ok);
        roads[15] = 0;
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
        check_build_settlement(0, 9);

        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(3)).type == ResType::Ok);
        settlements[3] = 0;
        settlements[9] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 5);

        REQUIRE(g->execute_build_road(g->player(0), g->road(7)).type == ResType::Ok);
        roads[7] = 0;
        check_roads();

        gs.roads_built += 1;
        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);

        REQUIRE(g->execute_choose_initial_resources(g->player(0), g->junction(3)).type == ResType::Ok);

        gs.is_second_round = false;
        gs.round += 1;
        gs.turn += 1;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].num_resources = 1;
        check_state();
        check_to_root(0);

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);

        REQUIRE(g->execute_roll_dice(g->player(0), 6).type == ResType::Ok);

        gs.has_rolled = true;
        gs.dice_total = 6;
        ps[0].num_resources += 2;
        check_state();
        check_end_turn(0);

        // dump_actions();
        delete g;
    }

    SECTION("Two players")
    {
        auto b = Board::Store::the().by_name(Board::Name::Triple);
        auto s = Scenario::Store::the().by_name(Scenario::Name::Triple);
        auto p = get_triple_parameters(2);
        auto g = Game::Game::initialize(b, s, p);

        bootstrap_tests();

        REQUIRE(g->players().size() == 2);

        gs.robber_location = 30;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;

        check_state();
        check_build_settlement(0, 13);
        check_no_actions(1);

        REQUIRE(g->execute_build_settlement(g->player(1), g->junction(20)).type == ResType::InvalidEdgeChoice);
        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(20)).type == ResType::Ok);
        settlements[20] = 0;
        settlements[10] = -1;
        settlements[26] = -1;
        settlements[27] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 3);
        check_no_actions(1);

        REQUIRE(g->execute_build_road(g->player(1), g->road(15)).type == ResType::InvalidEdgeChoice);
        REQUIRE(g->execute_build_road(g->player(0), g->road(15)).type == ResType::Ok);
        roads[15] = 0;
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

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::InvalidEdgeChoice);
        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 9);

        REQUIRE(g->execute_build_settlement(g->player(1), g->junction(3)).type == ResType::Ok);
        settlements[3] = 1;
        settlements[9] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 1;
        ps[1].public_victory_points = 1;
        check_state();
        check_no_actions(0);
        check_build_road(1, 2);

        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(4)).type == ResType::InvalidEdgeChoice);
        REQUIRE(g->execute_build_road(g->player(1), g->road(6)).type == ResType::Ok);
        roads[6] = 1;
        check_roads();

        gs.roads_built += 1;
        gs.is_first_round = false;
        gs.is_second_round = true;
        gs.round += 1;
        gs.turn += 1;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].roads = 1;
        check_state();
        check_no_actions(0);
        check_to_root(1);

        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::InvalidEdgeChoice);
        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 7);

        REQUIRE(g->execute_build_settlement(g->player(1), g->junction(33)).type == ResType::Ok);
        settlements[33] = 1;
        settlements[37] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 2;
        ps[1].public_victory_points = 2;
        check_state();
        check_no_actions(0);
        check_build_road(1, 4);

        REQUIRE(g->execute_build_road(g->player(1), g->road(29)).type == ResType::Ok);
        roads[29] = 1;
        check_roads();

        gs.roads_built += 1;
        ps[1].vertex = Vertex::ChooseInitialResources;
        ps[1].roads = 2;
        check_state();
        check_no_actions(0);
        check_choose_initial_resources(1);

        REQUIRE(g->execute_choose_initial_resources(g->player(1), g->junction(3)).type == ResType::Ok);

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
        check_build_settlement(0, 5);
        check_no_actions(1);

        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(34)).type == ResType::Ok);
        settlements[34] = 0;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 5);
        check_no_actions(1);

        REQUIRE(g->execute_build_road(g->player(0), g->road(7)).type == ResType::Ok);
        roads[7] = 0;
        check_roads();

        gs.roads_built += 1;
        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);
        check_no_actions(1);

        REQUIRE(g->execute_choose_initial_resources(g->player(0), g->junction(20)).type == ResType::Ok);

        gs.is_second_round = false;
        gs.turn += 1;
        gs.round += 1;
        ps[0].num_resources = 2;
        ps[0].vertex = Vertex::WaitForTurn;
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
        ps[0].num_resources += 1;
        ps[1].num_resources += 1;
        check_state();
        check_end_turn(0);

        // dump_actions();
        delete g;
    }

    SECTION("Three players")
    {
        auto b = Board::Store::the().by_name(Board::Name::Triple);
        auto s = Scenario::Store::the().by_name(Scenario::Name::Triple);
        auto p = get_triple_parameters(3);
        auto g = Game::Game::initialize(b, s, p);

        bootstrap_tests();

        REQUIRE(g->players().size() == 3);

        gs.robber_location = 30;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[2].vertex = Vertex::WaitForTurn;

        check_state();
        check_build_settlement(0, 13);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(3)).type == ResType::Ok);
        settlements[3] = 0;
        settlements[9] = -1;
        settlements[10] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 2);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_build_road(g->player(0), g->road(5)).type == ResType::Ok);
        roads[5] = 0;
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
        check_build_settlement(1, 10);

        REQUIRE(g->execute_build_settlement(g->player(1), g->junction(4)).type == ResType::Ok);
        settlements[4] = 1;
        settlements[11] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 1;
        ps[1].public_victory_points = 1;
        check_state();
        check_no_actions(0);
        check_build_road(1, 2);
        check_no_actions(2);

        REQUIRE(g->execute_build_road(g->player(1), g->road(7)).type == ResType::Ok);
        roads[7] = 1;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        ps[1].is_current_player = false;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].roads = 1;
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
        check_build_settlement(2, 8);

        REQUIRE(g->execute_build_settlement(g->player(2), g->junction(19)).type == ResType::Ok);
        settlements[19] = 2;
        settlements[26] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[2].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[2].settlements = 1;
        ps[2].public_victory_points = 1;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_road(2, 2);

        REQUIRE(g->execute_build_road(g->player(2), g->road(13)).type == ResType::Ok);
        roads[13] = 2;
        check_roads();

        gs.roads_built += 1;
        gs.is_first_round = false;
        gs.is_second_round = true;
        gs.turn += 1;
        gs.round += 1;
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
        check_build_settlement(2, 6);

        REQUIRE(g->execute_build_settlement(g->player(2), g->junction(20)).type == ResType::Ok);
        settlements[20] = 2;
        settlements[27] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[2].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[2].settlements = 2;
        ps[2].public_victory_points = 2;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_road(2, 4);

        REQUIRE(g->execute_build_road(g->player(2), g->road(15)).type == ResType::Ok);
        roads[15] = 2;
        check_roads();

        gs.roads_built += 1;
        ps[2].vertex = Vertex::ChooseInitialResources;
        ps[2].roads = 2;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_choose_initial_resources(2);

        REQUIRE(g->execute_choose_initial_resources(g->player(2), g->junction(20)).type == ResType::Ok);

        gs.turn += 1;
        ps[1].is_current_player = true;
        ps[2].vertex = Vertex::WaitForTurn;
        ps[2].is_current_player = false;
        ps[2].num_resources = 2;
        check_state();
        check_no_actions(0);
        check_to_root(1);
        check_no_actions(2);

        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 4);
        check_no_actions(2);

        REQUIRE(g->execute_build_settlement(g->player(1), g->junction(21)).type == ResType::Ok);
        settlements[21] = 1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 2;
        ps[1].public_victory_points = 2;
        check_state();
        check_no_actions(0);
        check_build_road(1, 4);
        check_no_actions(2);

        REQUIRE(g->execute_build_road(g->player(1), g->road(17)).type == ResType::Ok);
        roads[17] = 1;
        check_roads();

        gs.roads_built += 1;
        ps[1].vertex = Vertex::ChooseInitialResources;
        ps[1].roads = 2;
        check_state();
        check_no_actions(0);
        check_choose_initial_resources(1);
        check_no_actions(2);

        REQUIRE(g->execute_choose_initial_resources(g->player(1), g->junction(4)).type == ResType::Ok);

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
        check_build_settlement(0, 3);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(37)).type == ResType::Ok);
        settlements[37] = 0;
        settlements[33] = -1;
        settlements[34] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 3);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_build_road(g->player(0), g->road(6)).type == ResType::Ok);
        roads[6] = 0;
        check_roads();

        gs.roads_built += 1;
        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);
        check_no_actions(1);
        check_no_actions(2);

        REQUIRE(g->execute_choose_initial_resources(g->player(0), g->junction(37)).type == ResType::Ok);

        gs.is_second_round = false;
        gs.turn += 1;
        gs.round += 1;
        ps[0].vertex = Vertex::WaitForTurn;
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
        ps[0].num_resources += 1;
        ps[2].num_resources += 2;
        check_state();
        check_end_turn(0);

        // dump_actions();
        delete g;
    }
}
