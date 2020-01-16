#include <assert.h>
#include <iostream>

#include "Board/generated/StandardBoard.h"
#include "Core/Building.h"
#include "Core/DevelopmentCard.h"
#include "Core/Random.h"
#include "Core/Resource.h"
#include "Game/Game.h"
#include "Scenario/Standard.h"

using namespace k10engine;

int main(int, char**)
{
    Board::StandardBoard board;

    /*
    Board::Graph g(10);
    g.add_edge(2, Direction::Clock4, 3);
    assert(g[2]->has_edge(Direction::Clock4));
    assert(g[3]->has_edge(Direction::Clock10));
    g.add_edge(2, Direction::Clock6, 5);
    assert(g[2]->has_edge(Direction::Clock6));
    assert(g[5]->has_edge(Direction::Clock12));
    g.add_edge(3, Direction::Clock4, 7);
    assert(g[3]->has_edge(Direction::Clock4));
    assert(g[7]->has_edge(Direction::Clock10));
    g.add_edge(5, Direction::Clock2, 7);
    assert(g[5]->has_edge(Direction::Clock2));
    assert(g[7]->has_edge(Direction::Clock8));

    std::cout << g << std::endl;
    std::cout << "done!" << std::endl;
    / *
    (HexNode(14, HexNode::ResourceType::Normal)).dump();
    (HexNode(1, HexNode::ResourceType::Flippable)).dump();
    (HexNode(6, HexNode::ResourceType::Ocean)).dump();
    (JunctionNode(9)).dump();

    auto h = HexNode(420, HexNode::ResourceType::Normal);
    h.dump();
    for (auto it : h) {
        printf(".");
    }
    printf("\n");
    */

    std::cout << board.name() << std::endl;

    Scenario::Standard s;
    std::cout << (s.is_valid(Resource::Wood) ? "Wood is valid" : "Wood is not valid") << std::endl;
    std::cout << (s.is_valid(NonYieldingResource::Desert) ? "Desert is valid"
                                                          : "Desert is not valid")
              << std::endl;
    std::cout << (s.is_valid(Building::Settlement) ? "Settlements are valid"
                                                   : "Settlements are not valid")
              << std::endl;
    std::cout << (s.is_valid(DevelopmentCard::Knight) ? "Knights are valid"
                                                      : "Knights are not valid")
              << std::endl;

    Random::seed();
    for (int i = 0; i < 50; ++i)
        std::cout << Random::get_dice_roll() << " ";
    std::cout << std::endl;

    return 0;
}
