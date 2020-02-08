#include <stdexcept>

#include "Core/Building.h"
#include "Scenario/Scenario.h"
#include "Test/catch.h"

using namespace k10engine::Scenario;

TEST_CASE("Scenario initialization", "[Scenario]")
{
    SECTION("Trivial")
    {
        const auto s = Scenario(0, 0, 0, 0, {}, {}, {}, {}, {}, {});
        REQUIRE(s.min_players_count() == 0);
        REQUIRE(s.max_players_count() == 0);
        REQUIRE(s.min_victory_points_goal() == 0);
        REQUIRE(s.max_victory_points_goal() == 0);
        REQUIRE(s.building_costs().empty());
        REQUIRE(!s.is_valid(k10engine::Building::City));
        REQUIRE(!s.is_valid(k10engine::Building::DevelopmentCard));
        REQUIRE(!s.is_valid(k10engine::Building::Road));
        REQUIRE(!s.is_valid(k10engine::Building::Settlement));
        REQUIRE(!s.cost(k10engine::Building::City).has_value());
        REQUIRE(!s.cost(k10engine::Building::DevelopmentCard).has_value());
        REQUIRE(!s.cost(k10engine::Building::Road).has_value());
        REQUIRE(!s.cost(k10engine::Building::Settlement).has_value());
        REQUIRE(!s.max_count(k10engine::Building::City).has_value());
        REQUIRE(!s.max_count(k10engine::Building::DevelopmentCard).has_value());
        REQUIRE(!s.max_count(k10engine::Building::Road).has_value());
        REQUIRE(!s.max_count(k10engine::Building::Settlement).has_value());
        REQUIRE(s.development_card_counts().empty());
        REQUIRE(!s.is_valid(k10engine::DevelopmentCard::Knight));
        REQUIRE(!s.is_valid(k10engine::DevelopmentCard::Monopoly));
        REQUIRE(!s.is_valid(k10engine::DevelopmentCard::RoadBuilding));
        REQUIRE(!s.is_valid(k10engine::DevelopmentCard::VictoryPoint));
        REQUIRE(!s.is_valid(k10engine::DevelopmentCard::YearOfPlenty));
        REQUIRE(!s.count(k10engine::DevelopmentCard::Knight).has_value());
        REQUIRE(!s.count(k10engine::DevelopmentCard::Monopoly).has_value());
        REQUIRE(!s.count(k10engine::DevelopmentCard::RoadBuilding).has_value());
        REQUIRE(!s.count(k10engine::DevelopmentCard::VictoryPoint).has_value());
        REQUIRE(!s.count(k10engine::DevelopmentCard::YearOfPlenty).has_value());
        REQUIRE(s.resource_counts().empty());
        REQUIRE(!s.is_valid(k10engine::NonYieldingResource::Desert));
        REQUIRE(!s.is_valid(k10engine::Resource::Brick));
        REQUIRE(!s.is_valid(k10engine::Resource::Ore));
        REQUIRE(!s.is_valid(k10engine::Resource::Sheep));
        REQUIRE(!s.is_valid(k10engine::Resource::Wheat));
        REQUIRE(!s.is_valid(k10engine::Resource::Wood));
        REQUIRE(!s.count(k10engine::NonYieldingResource::Desert).has_value());
        REQUIRE(!s.count(k10engine::Resource::Brick).has_value());
        REQUIRE(!s.count(k10engine::Resource::Ore).has_value());
        REQUIRE(!s.count(k10engine::Resource::Sheep).has_value());
        REQUIRE(!s.count(k10engine::Resource::Wheat).has_value());
        REQUIRE(!s.count(k10engine::Resource::Wood).has_value());
        REQUIRE(s.get_development_card_deck(IterationType::Fixed).empty());
        REQUIRE(s.get_development_card_deck(IterationType::Random).empty());
        REQUIRE(s.get_ports(IterationType::Fixed).empty());
        REQUIRE(s.get_ports(IterationType::Random).empty());
        REQUIRE(s.get_resources(IterationType::Fixed).empty());
        REQUIRE(s.get_resources(IterationType::Random).empty());
        REQUIRE(s.get_rolls(IterationType::Fixed).empty());
        REQUIRE(s.get_rolls(IterationType::Random).empty());
    }

    /*
     Costs<Building> building_costs,
     Counts<Building> building_max_counts,
     Counts<DevelopmentCard> development_card_counts,
     Counts<AbstractResource> resource_counts,
     std::vector<int> rolls,
     std::vector<ResourceCollection> ports)
     */
    SECTION("with Costs<Building>")
    {
        SECTION("standard City")
        {
            const k10engine::ResourceCounts expected_cost = { { k10engine::Resource::Ore, 3 },
                                                              { k10engine::Resource::Wheat, 2 } };
            const Costs<k10engine::Building> costs = { { k10engine::Building::City,
                                                         expected_cost } };
            const auto s = Scenario(0, 0, 0, 0, costs, {}, {}, {}, {}, {});
            REQUIRE(!s.building_costs().empty());
            REQUIRE(s.building_costs().size() == 1);
            REQUIRE(s.is_valid(k10engine::Building::City));
            REQUIRE(!s.is_valid(k10engine::Building::DevelopmentCard));
            REQUIRE(!s.is_valid(k10engine::Building::Road));
            REQUIRE(!s.is_valid(k10engine::Building::Settlement));
            REQUIRE(s.cost(k10engine::Building::City).has_value());
            const auto actual_city_cost = s.cost(k10engine::Building::City).value();
            REQUIRE(actual_city_cost.size() == expected_cost.size());
            for (const auto it : actual_city_cost) {
                const auto actual_resource_type = it.first;
                const auto actual_resource_cost = it.second;
                REQUIRE(expected_cost.at(actual_resource_type) == actual_resource_cost);
            }
            REQUIRE(!s.cost(k10engine::Building::DevelopmentCard).has_value());
            REQUIRE(!s.cost(k10engine::Building::Road).has_value());
            REQUIRE(!s.cost(k10engine::Building::Settlement).has_value());
        }

        SECTION("standard DevelopmentCard")
        {
            const k10engine::ResourceCounts expected_cost = { { k10engine::Resource::Ore, 1 },
                                                              { k10engine::Resource::Sheep, 1 },
                                                              { k10engine::Resource::Wheat, 1 } };
            const Costs<k10engine::Building> costs = { { k10engine::Building::DevelopmentCard,
                                                         expected_cost } };
            const auto s = Scenario(0, 0, 0, 0, costs, {}, {}, {}, {}, {});
            REQUIRE(!s.building_costs().empty());
            REQUIRE(s.building_costs().size() == 1);
            REQUIRE(!s.is_valid(k10engine::Building::City));
            REQUIRE(s.is_valid(k10engine::Building::DevelopmentCard));
            REQUIRE(!s.is_valid(k10engine::Building::Road));
            REQUIRE(!s.is_valid(k10engine::Building::Settlement));
            REQUIRE(!s.cost(k10engine::Building::City).has_value());
            REQUIRE(s.cost(k10engine::Building::DevelopmentCard).has_value());
            const auto actual_cost = s.cost(k10engine::Building::DevelopmentCard).value();
            REQUIRE(actual_cost.size() == expected_cost.size());
            for (const auto it : actual_cost) {
                const auto actual_resource_type = it.first;
                const auto actual_resource_cost = it.second;
                REQUIRE(expected_cost.at(actual_resource_type) == actual_resource_cost);
            }
            REQUIRE(!s.cost(k10engine::Building::Road).has_value());
            REQUIRE(!s.cost(k10engine::Building::Settlement).has_value());
        }

        SECTION("standard Road")
        {
            const k10engine::ResourceCounts expected_cost = {
                { k10engine::Resource::Brick, 1 },
                { k10engine::Resource::Wood, 1 },
            };
            const Costs<k10engine::Building> costs = { { k10engine::Building::Road,
                                                         expected_cost } };
            const auto s = Scenario(0, 0, 0, 0, costs, {}, {}, {}, {}, {});
            REQUIRE(!s.building_costs().empty());
            REQUIRE(s.building_costs().size() == 1);
            REQUIRE(!s.is_valid(k10engine::Building::City));
            REQUIRE(!s.is_valid(k10engine::Building::DevelopmentCard));
            REQUIRE(s.is_valid(k10engine::Building::Road));
            REQUIRE(!s.is_valid(k10engine::Building::Settlement));
            REQUIRE(!s.cost(k10engine::Building::City).has_value());
            REQUIRE(!s.cost(k10engine::Building::DevelopmentCard).has_value());
            REQUIRE(s.cost(k10engine::Building::Road).has_value());
            const auto actual_cost = s.cost(k10engine::Building::Road).value();
            REQUIRE(actual_cost.size() == expected_cost.size());
            for (const auto it : actual_cost) {
                const auto actual_resource_type = it.first;
                const auto actual_resource_cost = it.second;
                REQUIRE(expected_cost.at(actual_resource_type) == actual_resource_cost);
            }
            REQUIRE(!s.cost(k10engine::Building::Settlement).has_value());
        }

        SECTION("standard Settlement")
        {
            const k10engine::ResourceCounts expected_cost = { { k10engine::Resource::Brick, 1 },
                                                              { k10engine::Resource::Sheep, 1 },
                                                              { k10engine::Resource::Wheat, 1 },
                                                              { k10engine::Resource::Wood, 1 } };
            const Costs<k10engine::Building> costs = { { k10engine::Building::Settlement,
                                                         expected_cost } };
            const auto s = Scenario(0, 0, 0, 0, costs, {}, {}, {}, {}, {});
            REQUIRE(!s.building_costs().empty());
            REQUIRE(s.building_costs().size() == 1);
            REQUIRE(!s.is_valid(k10engine::Building::City));
            REQUIRE(!s.is_valid(k10engine::Building::DevelopmentCard));
            REQUIRE(!s.is_valid(k10engine::Building::Road));
            REQUIRE(s.is_valid(k10engine::Building::Settlement));
            REQUIRE(!s.cost(k10engine::Building::City).has_value());
            REQUIRE(!s.cost(k10engine::Building::DevelopmentCard).has_value());
            REQUIRE(!s.cost(k10engine::Building::Road).has_value());
            REQUIRE(s.cost(k10engine::Building::Settlement).has_value());
            const auto actual_cost = s.cost(k10engine::Building::Settlement).value();
            REQUIRE(actual_cost.size() == expected_cost.size());
            for (const auto it : actual_cost) {
                const auto actual_resource_type = it.first;
                const auto actual_resource_cost = it.second;
                REQUIRE(expected_cost.at(actual_resource_type) == actual_resource_cost);
            }
        }

        SECTION("all standard buildings")
        {
            const Costs<k10engine::Building> costs = { {
                                                           k10engine::Building::City,
                                                           { { k10engine::Resource::Ore, 3 },
                                                             { k10engine::Resource::Wheat, 2 } },
                                                       },
                                                       {
                                                           k10engine::Building::DevelopmentCard,
                                                           { { k10engine::Resource::Ore, 1 },
                                                             { k10engine::Resource::Sheep, 1 },
                                                             { k10engine::Resource::Wheat, 1 } },
                                                       },
                                                       {
                                                           k10engine::Building::Road,
                                                           {
                                                               { k10engine::Resource::Brick, 1 },
                                                               { k10engine::Resource::Wood, 1 },
                                                           },
                                                       },
                                                       {
                                                           k10engine::Building::Settlement,
                                                           { { k10engine::Resource::Brick, 1 },
                                                             { k10engine::Resource::Sheep, 1 },
                                                             { k10engine::Resource::Wheat, 1 },
                                                             { k10engine::Resource::Wood, 1 } },
                                                       } };
            const auto s = Scenario(0, 0, 0, 0, costs, {}, {}, {}, {}, {});
            REQUIRE(!s.building_costs().empty());
            REQUIRE(s.building_costs().size() == 4);
            REQUIRE(s.is_valid(k10engine::Building::City));
            REQUIRE(s.is_valid(k10engine::Building::DevelopmentCard));
            REQUIRE(s.is_valid(k10engine::Building::Road));
            REQUIRE(s.is_valid(k10engine::Building::Settlement));
            REQUIRE(s.cost(k10engine::Building::City).has_value());
            REQUIRE(s.cost(k10engine::Building::DevelopmentCard).has_value());
            REQUIRE(s.cost(k10engine::Building::Road).has_value());
            REQUIRE(s.cost(k10engine::Building::Settlement).has_value());
            for (const auto resource_cost_spec : costs) {
                const auto building_type = resource_cost_spec.first;
                const auto actual_cost = resource_cost_spec.second;
                const auto expected_cost = costs.at(building_type);
                REQUIRE(actual_cost.size() == expected_cost.size());
                for (const auto it : actual_cost) {
                    const auto actual_resource_type = it.first;
                    const auto actual_resource_cost = it.second;
                    REQUIRE(expected_cost.at(actual_resource_type) == actual_resource_cost);
                }
            }
        }
    }
}

TEST_CASE("Parameter validation", "[Scenario]") {}
