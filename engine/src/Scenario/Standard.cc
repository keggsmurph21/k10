#include "Scenario/Standard.h"

namespace k10engine {

namespace Scenario {

Standard::Standard()
{
    m_min_players_count = 2;
    m_max_players_count = 6;

    m_min_victory_points_goal = 6;
    m_max_victory_points_goal = 12;

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
}

} // namespace Scenario

} // namespace k10engine
