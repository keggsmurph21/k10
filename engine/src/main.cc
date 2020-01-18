#include <assert.h>
#include <iostream>
#include <set>
#include <tuple>
#include <vector>

#include "Board/Base.h"
#include "Board/generated/Portless.h"
#include "Board/generated/Standard.h"
#include "Board/generated/Tall.h"

/*
#include "Board/generated/Standard.h"
#include "Core/Building.h"
#include "Core/DevelopmentCard.h"
#include "Core/Random.h"
#include "Core/Resource.h"
#include "Game/Game.h"
#include "Scenario/Scenario.h"
*/

using namespace k10engine::Board;

int main(int, char**)
{
    auto n0 = Node(0, NodeType::Ocean);
    assert(n0.index() == 0);
    assert(n0.type() == NodeType::Ocean);
    assert(!n0.has_edge(Direction::Clock2));
    assert(!n0.has_edge(Direction::Clock4));
    assert(!n0.has_edge(Direction::Clock6));
    assert(!n0.has_edge(Direction::Clock8));
    assert(!n0.has_edge(Direction::Clock10));
    assert(!n0.has_edge(Direction::Clock12));
    assert(!n0.get_edge(Direction::Clock2));
    assert(!n0.get_edge(Direction::Clock4));
    assert(!n0.get_edge(Direction::Clock6));
    assert(!n0.get_edge(Direction::Clock8));
    assert(!n0.get_edge(Direction::Clock10));
    assert(!n0.get_edge(Direction::Clock12));
    assert(n0.neighbors()->size() == 0);
    std::cout << n0 << std::endl;

    // NB: The EdgeList passed to the Graph constructor must include
    //     both directions in order for this to be an undirected graph!
    auto n1 = Node(1, NodeType::Junction);
    n0.add_edge(Direction::Clock2, &n1);
    assert(n0.has_edge(Direction::Clock2));
    assert(n0.get_edge(Direction::Clock2) == &n1);
    assert(n0.neighbors()->size() == 1);
    assert(!n1.has_edge(Direction::Clock2));
    assert(!n1.get_edge(Direction::Clock2));
    assert(n1.neighbors()->size() == 0);
    std::cout << n1 << std::endl;

    // NB: These would throw:
    /*
    Port(std::set<const Node*>({ nullptr, nullptr }), Orientation::Clock2Clock8);
    Port(std::set<const Node*>({ &n0, &n1 }), Orientation::Clock2Clock8);
    */
    const auto n2 = Node(2, NodeType::Junction);
    Port(std::set<const Node*>({ &n1, &n2 }), Orientation::Clock2Clock8);

    auto g0 = Graph({}, {});
    assert(g0.num_hexes() == 0);
    assert(g0.num_junctions() == 0);
    assert(g0.num_oceans() == 0);
    assert(g0.num_roads() == 0);
    assert(g0.num_unflipped_hexes() == 0);
    assert(!g0.node(0));

    auto g1 = Graph({ { 0, NodeType::Ocean } }, {});
    assert(g1.num_hexes() == 0);
    assert(g1.num_junctions() == 0);
    assert(g1.num_oceans() == 1);
    assert(g1.num_roads() == 0);
    assert(g1.num_unflipped_hexes() == 0);
    auto g1_node0 = g1.node(0);
    assert(g1_node0);
    assert(g1_node0->index() == 0);
    assert(g1_node0->type() == NodeType::Ocean);

    // NB: These should throw:
    /*
    (void)Graph({ { 0, NodeType::Ocean }, { 0, NodeType::Ocean } }, {});
    (void)Graph({}, { { 0, 1, Direction::Clock2 } });
    (void)Graph({ { 0, NodeType::Ocean }, { 1, NodeType::Junction }, { 2, NodeType::Road } },
                { { 0, 1, Direction::Clock2 }, { 0, 2, Direction::Clock2 } });
    */

    auto g2 = Graph({ { 0, NodeType::Ocean }, { 1, NodeType::Junction } },
                    { { 0, 1, Direction::Clock2 } });
    assert(g2.num_hexes() == 0);
    assert(g2.num_junctions() == 1);
    assert(g2.num_oceans() == 1);
    assert(g2.num_roads() == 0);
    assert(g2.num_unflipped_hexes() == 0);
    auto g2_node0 = g2.node(0);
    assert(g2_node0);
    assert(g2_node0->index() == 0);
    assert(g2_node0->type() == NodeType::Ocean);
    auto g2_node1 = g2.node(1);
    assert(g2_node1);
    assert(g2_node1->index() == 1);
    assert(g2_node1->type() == NodeType::Junction);

    auto b0 = Base("test", new Graph({}, {}), {});
    auto b1 = Base(
        "test",
        new Graph({ { 1, NodeType::Junction }, { 2, NodeType::Road }, { 3, NodeType::Junction } },
                  { { 1, 2, Direction::Clock2 },
                    { 2, 1, Direction::Clock8 },
                    { 2, 3, Direction::Clock2 },
                    { 3, 2, Direction::Clock8 } }),
        { { 1, 3, Orientation::Clock2Clock8 } });

    auto standard_board = get_standard_board();
    delete standard_board;

    auto portless_board = get_portless_board();
    delete portless_board;

    auto tall_board = get_tall_board();
    delete tall_board;

    /*
    Board::Standard b;

    Scenario::Costs<Building> building_costs;
    building_costs[Building::City] = { { Resource::Ore, 3 }, { Resource::Wheat, 2 } };
    building_costs[Building::DevelopmentCard] = { { Resource::Ore, 1 },
                                                  { Resource::Sheep, 1 },
                                                  { Resource::Wheat, 1 } };
    building_costs[Building::Settlement] = { { Resource::Brick, 1 },
                                             { Resource::Sheep, 1 },
                                             { Resource::Wheat, 1 },
                                             { Resource::Wood, 1 } };
    building_costs[Building::Road] = { { Resource::Brick, 1 }, { Resource::Wood, 1 } };

    Scenario::Counts<Building> building_max_counts;
    building_max_counts[Building::City] = 4;
    building_max_counts[Building::DevelopmentCard] = 25;
    building_max_counts[Building::Settlement] = 5;
    building_max_counts[Building::Road] = 15;

    Scenario::Counts<DevelopmentCard> development_card_counts;
    development_card_counts[DevelopmentCard::Knight] = 14;
    development_card_counts[DevelopmentCard::Monopoly] = 2;
    development_card_counts[DevelopmentCard::RoadBuilding] = 2;
    development_card_counts[DevelopmentCard::VictoryPoint] = 5;
    development_card_counts[DevelopmentCard::YearOfPlenty] = 2;

    Scenario::Counts<AbstractResource> resource_counts;
    resource_counts[NonYieldingResource::Desert] = 1;
    resource_counts[Resource::Brick] = 3;
    resource_counts[Resource::Ore] = 3;
    resource_counts[Resource::Sheep] = 4;
    resource_counts[Resource::Wheat] = 4;
    resource_counts[Resource::Wood] = 4;

    std::vector<int> rolls = {
        9, 5, 2, 6, 3, 8, 10, 12, 11, 4, 8, 10, 9, 4, 5, 6, 3, 11,
    };

    std::vector<ResourceCollection> ports = {
        { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat,
    Resource::Wood }, { Resource::Wheat }, { Resource::Ore }, { Resource::Wood }, {
    Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood },
        { Resource::Brick },
        { Resource::Sheep },
        { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat,
    Resource::Wood }, { Resource::Brick, Resource::Ore, Resource::Sheep,
    Resource::Wheat, Resource::Wood },
    };

    Scenario::Scenario* s = new Scenario::Scenario(2,
                                                   6,
                                                   6,
                                                   12,
                                                   building_costs,
                                                   building_max_counts,
                                                   development_card_counts,
                                                   resource_counts,
                                                   rolls,
                                                   ports);

    Scenario::Parameters p{
        Scenario::IterationType::Random,
        Scenario::IterationType::Random,
        Scenario::IterationType::Random,
        Scenario::IterationType::Random,
        3,
        10,
    };

    auto g = Game::Game::initialize(&b, s, &p);

    if (g.has_value()) {
        std::cout << "Survived!" << std::endl;
    } else {
        std::cout << "Died!" << std::endl;
    }

    return 0;
    */
}
