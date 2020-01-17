#include "Scenario/Standard.h"

namespace k10engine {

namespace Scenario {

Standard::Standard()
{
    m_building_costs[Building::City] = { { Resource::Ore, 3 }, { Resource::Wheat, 2 } };
    m_building_costs[Building::DevelopmentCard] = { { Resource::Ore, 1 },
                                                    { Resource::Sheep, 1 },
                                                    { Resource::Wheat, 1 } };
    m_building_costs[Building::Settlement] = { { Resource::Brick, 1 },
                                               { Resource::Sheep, 1 },
                                               { Resource::Wheat, 1 },
                                               { Resource::Wood, 1 } };
    m_building_costs[Building::Road] = { { Resource::Brick, 1 }, { Resource::Wood, 1 } };

    m_building_max_counts[Building::City] = 4;
    m_building_max_counts[Building::DevelopmentCard] = 25;
    m_building_max_counts[Building::Settlement] = 5;
    m_building_max_counts[Building::Road] = 15;

    m_development_card_counts[DevelopmentCard::Knight] = 14;
    m_development_card_counts[DevelopmentCard::Monopoly] = 2;
    m_development_card_counts[DevelopmentCard::RoadBuilding] = 2;
    m_development_card_counts[DevelopmentCard::VictoryPoint] = 5;
    m_development_card_counts[DevelopmentCard::YearOfPlenty] = 2;

    m_resource_counts[NonYieldingResource::Desert] = 1;
    m_resource_counts[Resource::Brick] = 3;
    m_resource_counts[Resource::Ore] = 3;
    m_resource_counts[Resource::Sheep] = 4;
    m_resource_counts[Resource::Wheat] = 4;
    m_resource_counts[Resource::Wood] = 4;

    m_ports.push_back(
        { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood });
    m_ports.push_back({ Resource::Wheat });
    m_ports.push_back({ Resource::Ore });
    m_ports.push_back({ Resource::Wood });
    m_ports.push_back(
        { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood });
    m_ports.push_back({ Resource::Brick });
    m_ports.push_back({ Resource::Sheep });
    m_ports.push_back(
        { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood });
    m_ports.push_back(
        { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood });

    m_rolls.push_back(9);
    m_rolls.push_back(5);
    m_rolls.push_back(2);
    m_rolls.push_back(6);
    m_rolls.push_back(3);
    m_rolls.push_back(8);
    m_rolls.push_back(10);
    m_rolls.push_back(12);
    m_rolls.push_back(11);
    m_rolls.push_back(4);
    m_rolls.push_back(8);
    m_rolls.push_back(10);
    m_rolls.push_back(9);
    m_rolls.push_back(4);
    m_rolls.push_back(5);
    m_rolls.push_back(6);
    m_rolls.push_back(3);
    m_rolls.push_back(11);
}

} // namespace Scenario

} // namespace k10engine
