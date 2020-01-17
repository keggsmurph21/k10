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
    Board::StandardBoard b;
    Scenario::Standard s;
    Scenario::Parameters p{
        Scenario::IterationType::Fixed,
        Scenario::IterationType::Fixed,
        Scenario::IterationType::Fixed,
        Scenario::IterationType::Fixed,
        3,
        10,
    };

    auto g = Game::Game::initialize(&b, &s, &p);

    if (g.has_value()) {
        std::cout << "Survived!" << std::endl;
    } else {
        std::cout << "Died!" << std::endl;
    }

    return 0;
}
