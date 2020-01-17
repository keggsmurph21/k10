#include <assert.h>
#include <iostream>

#include "Board/generated/Standard.h"
#include "Core/Building.h"
#include "Core/DevelopmentCard.h"
#include "Core/Random.h"
#include "Core/Resource.h"
#include "Game/Game.h"
#include "Scenario/Scenario.h"

using namespace k10engine;

int main(int, char**)
{
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
        { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood },
        { Resource::Wheat },
        { Resource::Ore },
        { Resource::Wood },
        { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood },
        { Resource::Brick },
        { Resource::Sheep },
        { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood },
        { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood },
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
        Scenario::IterationType::Fixed,
        Scenario::IterationType::Fixed,
        Scenario::IterationType::Fixed,
        Scenario::IterationType::Fixed,
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
}
