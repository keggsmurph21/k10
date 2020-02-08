#include <iostream>
#include <stdexcept>
#include <vector>

#include "Core/Building.h"
#include "Core/Random.h"
#include "Scenario/Scenario.h"
#include "Test/catch.h"

namespace k10engine {

namespace Scenario {
/*
 Costs<Building> building_costs,
 Counts<Building> building_counts,
 Counts<DevelopmentCard> development_card_counts,
 Counts<AbstractResource> resource_counts,
 std::vector<int> rolls,
 std::vector<ResourceCollection> ports)
 */

// NOLINTNEXTLINE(readability-function-size,google-readability-function-size)
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
        REQUIRE(s.count(k10engine::Building::City) == 0);
        REQUIRE(s.count(k10engine::Building::DevelopmentCard) == 0);
        REQUIRE(s.count(k10engine::Building::Road) == 0);
        REQUIRE(s.count(k10engine::Building::Settlement) == 0);
        REQUIRE(s.development_card_counts().empty());
        REQUIRE(!s.is_valid(k10engine::DevelopmentCard::Knight));
        REQUIRE(!s.is_valid(k10engine::DevelopmentCard::Monopoly));
        REQUIRE(!s.is_valid(k10engine::DevelopmentCard::RoadBuilding));
        REQUIRE(!s.is_valid(k10engine::DevelopmentCard::VictoryPoint));
        REQUIRE(!s.is_valid(k10engine::DevelopmentCard::YearOfPlenty));
        REQUIRE(s.count(k10engine::DevelopmentCard::Knight) == 0);
        REQUIRE(s.count(k10engine::DevelopmentCard::Monopoly) == 0);
        REQUIRE(s.count(k10engine::DevelopmentCard::RoadBuilding) == 0);
        REQUIRE(s.count(k10engine::DevelopmentCard::VictoryPoint) == 0);
        REQUIRE(s.count(k10engine::DevelopmentCard::YearOfPlenty) == 0);
        REQUIRE(s.resource_counts().empty());
        REQUIRE(!s.is_valid(k10engine::NonYieldingResource::Desert));
        REQUIRE(!s.is_valid(k10engine::Resource::Brick));
        REQUIRE(!s.is_valid(k10engine::Resource::Ore));
        REQUIRE(!s.is_valid(k10engine::Resource::Sheep));
        REQUIRE(!s.is_valid(k10engine::Resource::Wheat));
        REQUIRE(!s.is_valid(k10engine::Resource::Wood));
        REQUIRE(s.count(k10engine::NonYieldingResource::Desert) == 0);
        REQUIRE(s.count(k10engine::Resource::Brick) == 0);
        REQUIRE(s.count(k10engine::Resource::Ore) == 0);
        REQUIRE(s.count(k10engine::Resource::Sheep) == 0);
        REQUIRE(s.count(k10engine::Resource::Wheat) == 0);
        REQUIRE(s.count(k10engine::Resource::Wood) == 0);
        REQUIRE(s.get_development_card_deck(IterationType::Fixed).empty());
        REQUIRE(s.get_development_card_deck(IterationType::Random).empty());
        REQUIRE(s.get_ports(IterationType::Fixed).empty());
        REQUIRE(s.get_ports(IterationType::Random).empty());
        REQUIRE(s.get_resources(IterationType::Fixed).empty());
        REQUIRE(s.get_resources(IterationType::Random).empty());
        REQUIRE(s.get_rolls(IterationType::Fixed).empty());
        REQUIRE(s.get_rolls(IterationType::Random).empty());
    }

    SECTION("Costs<Building>")
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
            for (const auto& resource_cost_spec : costs) {
                const auto building_type = resource_cost_spec.first;
                const auto actual_cost = resource_cost_spec.second;
                const auto& expected_cost = costs.at(building_type);
                REQUIRE(actual_cost.size() == expected_cost.size());
                for (const auto& it : actual_cost) {
                    const auto actual_resource_type = it.first;
                    const auto actual_resource_cost = it.second;
                    REQUIRE(expected_cost.at(actual_resource_type) == actual_resource_cost);
                }
            }
        }
    }

    SECTION("Counts<Building>")
    {
        const std::vector<k10engine::Building> buildings = { k10engine::Building::City,
                                                             k10engine::Building::DevelopmentCard,
                                                             k10engine::Building::Road,
                                                             k10engine::Building::Settlement };
        for (const auto& building : buildings) {
            const auto s =
                Scenario(0, 0, 0, 0, { { building, {} } }, { { building, 1 } }, {}, {}, {}, {});
            REQUIRE(!s.building_counts().empty());
            for (const auto& other_building : buildings) {
                const auto is_match = building == other_building;
                REQUIRE(s.is_valid(other_building) == is_match);
                REQUIRE(s.count(other_building) == (is_match ? 1 : 0));
            }
        }
    }

    SECTION("Counts<DevelopmentCard>")
    {
        const std::vector<k10engine::DevelopmentCard> development_cards = {
            k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Monopoly,
            k10engine::DevelopmentCard::RoadBuilding, k10engine::DevelopmentCard::VictoryPoint,
            k10engine::DevelopmentCard::YearOfPlenty,
        };
        for (const auto& development_card : development_cards) {
            const auto s = Scenario(0, 0, 0, 0, {}, {}, { { development_card, 1 } }, {}, {}, {});
            REQUIRE(!s.development_card_counts().empty());
            for (const auto& other_development_card : development_cards) {
                const auto is_match = development_card == other_development_card;
                REQUIRE(s.is_valid(other_development_card) == is_match);
                REQUIRE(s.count(other_development_card) == (is_match ? 1 : 0));
            }
        }
    }

    SECTION("Counts<AbstractResource>")
    {
        const std::vector<k10engine::AbstractResource> resources = {
            NonYieldingResource::Desert,
            Resource::Brick,
            Resource::Ore,
            Resource::Sheep,
            Resource::Wheat,
            Resource::Wood,
        };
        for (const auto& resource : resources) {
            const auto s = Scenario(0, 0, 0, 0, {}, {}, {}, { { resource, 1 } }, {}, {});
            REQUIRE(!s.resource_counts().empty());
            for (const auto& other_resource : resources) {
                const auto is_match = resource == other_resource;
                REQUIRE(s.is_valid(other_resource) == is_match);
                REQUIRE(s.count(other_resource) == (is_match ? 1 : 0));
            }
        }
    }
}

// NOLINTNEXTLINE(readability-function-size,google-readability-function-size)
TEST_CASE("Iteration of (possibly) randomly generated values", "[Scenario]")
{
    SECTION("get_resources()")
    {
        SECTION("Trivial")
        {
            const auto s = Scenario(0, 0, 0, 0, {}, {}, {}, {}, {}, {});
            REQUIRE(s.get_resources(IterationType::Fixed).empty());
            REQUIRE(s.get_resources(IterationType::Random).empty());
        }

        SECTION("1 resource")
        {
            const auto s =
                Scenario(0, 0, 0, 0, {}, {}, {}, { { k10engine::Resource::Ore, 1 } }, {}, {});
            const auto& fixed_resources = s.get_resources(IterationType::Fixed);
            REQUIRE(!fixed_resources.empty());
            REQUIRE(fixed_resources.size() == 1);
            REQUIRE(std::get<k10engine::Resource>(fixed_resources.at(0))
                    == k10engine::Resource::Ore);
            const auto& random_resources = s.get_resources(IterationType::Random);
            REQUIRE(!random_resources.empty());
            REQUIRE(random_resources.size() == 1);
            REQUIRE(std::get<k10engine::Resource>(random_resources.at(0))
                    == k10engine::Resource::Ore);
        }

        SECTION("2 resources (same)")
        {
            const auto s =
                Scenario(0, 0, 0, 0, {}, {}, {}, { { k10engine::Resource::Ore, 2 } }, {}, {});
            const auto& fixed_resources = s.get_resources(IterationType::Fixed);
            REQUIRE(!fixed_resources.empty());
            REQUIRE(fixed_resources.size() == 2);
            REQUIRE(std::get<k10engine::Resource>(fixed_resources.at(0))
                    == k10engine::Resource::Ore);
            REQUIRE(std::get<k10engine::Resource>(fixed_resources.at(1))
                    == k10engine::Resource::Ore);
            const auto& random_resources = s.get_resources(IterationType::Random);
            REQUIRE(!random_resources.empty());
            REQUIRE(random_resources.size() == 2);
            REQUIRE(std::get<k10engine::Resource>(random_resources.at(0))
                    == k10engine::Resource::Ore);
            REQUIRE(std::get<k10engine::Resource>(random_resources.at(1))
                    == k10engine::Resource::Ore);
        }

        SECTION("2 resources (different)")
        {
            k10engine::Random::seed(42);
            const auto s =
                Scenario(0,
                         0,
                         0,
                         0,
                         {},
                         {},
                         {},
                         { { k10engine::Resource::Ore, 1 }, { k10engine::Resource::Wheat, 1 } },
                         {},
                         {});
            const auto& fixed_resources = s.get_resources(IterationType::Fixed);
            REQUIRE(!fixed_resources.empty());
            REQUIRE(fixed_resources.size() == 2);
            REQUIRE(std::get<k10engine::Resource>(fixed_resources.at(0))
                    == k10engine::Resource::Ore);
            REQUIRE(std::get<k10engine::Resource>(fixed_resources.at(1))
                    == k10engine::Resource::Wheat);
            const auto& random_resources = s.get_resources(IterationType::Random);
            REQUIRE(!random_resources.empty());
            REQUIRE(random_resources.size() == 2);
            REQUIRE(std::get<k10engine::Resource>(random_resources.at(0))
                    == k10engine::Resource::Wheat);
            REQUIRE(std::get<k10engine::Resource>(random_resources.at(1))
                    == k10engine::Resource::Ore);
        }

        SECTION("standard resources")
        {
            k10engine::Random::seed(42);
            const auto s = Scenario(0,
                                    0,
                                    0,
                                    0,
                                    {},
                                    {},
                                    {},
                                    { { k10engine::NonYieldingResource::Desert, 1 },
                                      { k10engine::Resource::Brick, 3 },
                                      { k10engine::Resource::Ore, 3 },
                                      { k10engine::Resource::Sheep, 4 },
                                      { k10engine::Resource::Wheat, 4 },
                                      { k10engine::Resource::Wood, 4 } },
                                    {},
                                    {});
            std::vector<k10engine::AbstractResource> expected_resources;
            std::vector<k10engine::AbstractResource> actual_resources;
            expected_resources = {
                k10engine::NonYieldingResource::Desert,
                k10engine::Resource::Brick,
                k10engine::Resource::Brick,
                k10engine::Resource::Brick,
                k10engine::Resource::Ore,
                k10engine::Resource::Ore,
                k10engine::Resource::Ore,
                k10engine::Resource::Sheep,
                k10engine::Resource::Sheep,
                k10engine::Resource::Sheep,
                k10engine::Resource::Sheep,
                k10engine::Resource::Wheat,
                k10engine::Resource::Wheat,
                k10engine::Resource::Wheat,
                k10engine::Resource::Wheat,
                k10engine::Resource::Wood,
                k10engine::Resource::Wood,
                k10engine::Resource::Wood,
                k10engine::Resource::Wood,
            };
            actual_resources = s.get_resources(IterationType::Fixed);
            REQUIRE(!actual_resources.empty());
            REQUIRE(actual_resources.size() == expected_resources.size());
            // NOLINTNEXTLINE(modernize-loop-convert)
            for (int i = 0; i < expected_resources.size(); ++i) {
                const auto expected_resource = expected_resources.at(i);
                const auto actual_resource = expected_resources.at(i);
                REQUIRE(actual_resource == expected_resource);
            }
            expected_resources = {
                k10engine::Resource::Ore,   k10engine::Resource::Sheep,
                k10engine::Resource::Wood,  k10engine::Resource::Wheat,
                k10engine::Resource::Wheat, k10engine::Resource::Wood,
                k10engine::Resource::Brick, k10engine::Resource::Sheep,
                k10engine::Resource::Wheat, k10engine::Resource::Wood,
                k10engine::Resource::Ore,   k10engine::Resource::Brick,
                k10engine::Resource::Sheep, k10engine::Resource::Sheep,
                k10engine::Resource::Ore,   k10engine::NonYieldingResource::Desert,
                k10engine::Resource::Wood,  k10engine::Resource::Brick,
                k10engine::Resource::Wheat,
            };
            actual_resources = s.get_resources(IterationType::Random);
            REQUIRE(!actual_resources.empty());
            REQUIRE(actual_resources.size() == expected_resources.size());
            // NOLINTNEXTLINE(modernize-loop-convert)
            for (int i = 0; i < expected_resources.size(); ++i) {
                const auto expected_resource = expected_resources.at(i);
                const auto actual_resource = expected_resources.at(i);
                REQUIRE(actual_resource == expected_resource);
            }
        }
    }

    SECTION("get_development_card_deck()")
    {
        SECTION("Trivial")
        {
            const auto s = Scenario(0, 0, 0, 0., {}, {}, {}, {}, {}, {});
            REQUIRE(s.get_development_card_deck(IterationType::Fixed).empty());
            REQUIRE(s.get_development_card_deck(IterationType::Random).empty());
        }

        SECTION("1 development card")
        {
            const auto s = Scenario(
                0, 0, 0, 0, {}, {}, { { k10engine::DevelopmentCard::Knight, 1 } }, {}, {}, {});
            const auto& fixed_development_cards = s.get_development_card_deck(IterationType::Fixed);
            REQUIRE(!fixed_development_cards.empty());
            REQUIRE(fixed_development_cards.size() == 1);
            REQUIRE(fixed_development_cards.at(0) == k10engine::DevelopmentCard::Knight);
            const auto& random_development_cards =
                s.get_development_card_deck(IterationType::Random);
            REQUIRE(!random_development_cards.empty());
            REQUIRE(random_development_cards.size() == 1);
            REQUIRE(random_development_cards.at(0) == k10engine::DevelopmentCard::Knight);
        }

        SECTION("2 development cards (same)")
        {
            const auto s = Scenario(
                0, 0, 0, 0, {}, {}, { { k10engine::DevelopmentCard::Knight, 2 } }, {}, {}, {});
            const auto& fixed_development_cards = s.get_development_card_deck(IterationType::Fixed);
            REQUIRE(!fixed_development_cards.empty());
            REQUIRE(fixed_development_cards.size() == 2);
            REQUIRE(fixed_development_cards.at(0) == k10engine::DevelopmentCard::Knight);
            REQUIRE(fixed_development_cards.at(1) == k10engine::DevelopmentCard::Knight);
            const auto& random_development_cards =
                s.get_development_card_deck(IterationType::Random);
            REQUIRE(!random_development_cards.empty());
            REQUIRE(random_development_cards.size() == 2);
            REQUIRE(random_development_cards.at(0) == k10engine::DevelopmentCard::Knight);
            REQUIRE(random_development_cards.at(1) == k10engine::DevelopmentCard::Knight);
        }

        SECTION("2 development cards (different)")
        {
            k10engine::Random::seed(42);
            const auto s = Scenario(0,
                                    0,
                                    0,
                                    0,
                                    {},
                                    {},
                                    { { k10engine::DevelopmentCard::Knight, 1 },
                                      { k10engine::DevelopmentCard::Monopoly, 1 } },
                                    {},
                                    {},
                                    {});
            const auto& fixed_development_cards = s.get_development_card_deck(IterationType::Fixed);
            REQUIRE(!fixed_development_cards.empty());
            REQUIRE(fixed_development_cards.size() == 2);
            REQUIRE(fixed_development_cards.at(0) == k10engine::DevelopmentCard::Knight);
            REQUIRE(fixed_development_cards.at(1) == k10engine::DevelopmentCard::Monopoly);
            const auto& random_development_cards =
                s.get_development_card_deck(IterationType::Random);
            REQUIRE(!random_development_cards.empty());
            REQUIRE(random_development_cards.size() == 2);
            REQUIRE(random_development_cards.at(0) == k10engine::DevelopmentCard::Monopoly);
            REQUIRE(random_development_cards.at(1) == k10engine::DevelopmentCard::Knight);
        }

        SECTION("standard development cards")
        {
            const auto s = Scenario(0,
                                    0,
                                    0,
                                    0,
                                    {},
                                    {},
                                    {
                                        { k10engine::DevelopmentCard::Knight, 14 },
                                        { k10engine::DevelopmentCard::Monopoly, 2 },
                                        { k10engine::DevelopmentCard::RoadBuilding, 2 },
                                        { k10engine::DevelopmentCard::VictoryPoint, 5 },
                                        { k10engine::DevelopmentCard::YearOfPlenty, 2 },
                                    },
                                    {},
                                    {},
                                    {});
            std::vector<k10engine::DevelopmentCard> expected_development_cards;
            std::vector<k10engine::DevelopmentCard> actual_development_cards;
            expected_development_cards = {
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Knight,
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Knight,
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Knight,
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Knight,
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Knight,
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Knight,
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Knight,
                k10engine::DevelopmentCard::Monopoly,     k10engine::DevelopmentCard::Monopoly,
                k10engine::DevelopmentCard::RoadBuilding, k10engine::DevelopmentCard::RoadBuilding,
                k10engine::DevelopmentCard::VictoryPoint, k10engine::DevelopmentCard::VictoryPoint,
                k10engine::DevelopmentCard::VictoryPoint, k10engine::DevelopmentCard::VictoryPoint,
                k10engine::DevelopmentCard::VictoryPoint, k10engine::DevelopmentCard::YearOfPlenty,
                k10engine::DevelopmentCard::YearOfPlenty,
            };
            actual_development_cards = s.get_development_card_deck(IterationType::Fixed);
            REQUIRE(!actual_development_cards.empty());
            REQUIRE(actual_development_cards.size() == expected_development_cards.size());
            // NOLINTNEXTLINE(modernize-loop-convert)
            for (int i = 0; i < expected_development_cards.size(); ++i) {
                const auto expected_development_card = expected_development_cards.at(i);
                const auto actual_development_card = expected_development_cards.at(i);
                REQUIRE(actual_development_card == expected_development_card);
            }
            expected_development_cards = {
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Knight,
                k10engine::DevelopmentCard::YearOfPlenty, k10engine::DevelopmentCard::VictoryPoint,
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Monopoly,
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Knight,
                k10engine::DevelopmentCard::VictoryPoint, k10engine::DevelopmentCard::YearOfPlenty,
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Knight,
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Knight,
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::VictoryPoint,
                k10engine::DevelopmentCard::Monopoly,     k10engine::DevelopmentCard::Knight,
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::VictoryPoint,
                k10engine::DevelopmentCard::Knight,       k10engine::DevelopmentCard::Knight,
                k10engine::DevelopmentCard::RoadBuilding, k10engine::DevelopmentCard::RoadBuilding,
                k10engine::DevelopmentCard::VictoryPoint,
            };
            actual_development_cards = s.get_development_card_deck(IterationType::Random);
            REQUIRE(!actual_development_cards.empty());
            REQUIRE(actual_development_cards.size() == expected_development_cards.size());
            // NOLINTNEXTLINE(modernize-loop-convert)
            for (int i = 0; i < expected_development_cards.size(); ++i) {
                const auto expected_development_card = expected_development_cards.at(i);
                const auto actual_development_card = expected_development_cards.at(i);
                REQUIRE(actual_development_card == expected_development_card);
            }
        }
    }

    SECTION("get_rolls()")
    {
        SECTION("Trivial")
        {
            const auto s = Scenario(0, 0, 0, 0., {}, {}, {}, {}, {}, {});
            REQUIRE(s.get_rolls(IterationType::Fixed).empty());
            REQUIRE(s.get_rolls(IterationType::Random).empty());
        }

        SECTION("1 roll")
        {
            const auto s = Scenario(0, 0, 0, 0, {}, {}, {}, {}, { 69 }, {});
            const auto& fixed_rolls = s.get_rolls(IterationType::Fixed);
            REQUIRE(!fixed_rolls.empty());
            REQUIRE(fixed_rolls.size() == 1);
            REQUIRE(fixed_rolls.at(0) == 69);
            const auto& random_rolls = s.get_rolls(IterationType::Random);
            REQUIRE(!random_rolls.empty());
            REQUIRE(random_rolls.size() == 1);
            REQUIRE(random_rolls.at(0) == 69);
        }

        SECTION("2 rolls (same)")
        {
            const auto s = Scenario(0, 0, 0, 0, {}, {}, {}, {}, { 69, 69 }, {});
            const auto& fixed_rolls = s.get_rolls(IterationType::Fixed);
            REQUIRE(!fixed_rolls.empty());
            REQUIRE(fixed_rolls.size() == 2);
            REQUIRE(fixed_rolls.at(0) == 69);
            REQUIRE(fixed_rolls.at(1) == 69);
            const auto& random_rolls = s.get_rolls(IterationType::Random);
            REQUIRE(!random_rolls.empty());
            REQUIRE(random_rolls.size() == 2);
            REQUIRE(random_rolls.at(0) == 69);
            REQUIRE(random_rolls.at(1) == 69);
        }

        SECTION("2 rolls (different)")
        {
            k10engine::Random::seed(42);
            const auto s = Scenario(0, 0, 0, 0, {}, {}, {}, {}, { 69, 420 }, {});
            const auto& fixed_rolls = s.get_rolls(IterationType::Fixed);
            REQUIRE(!fixed_rolls.empty());
            REQUIRE(fixed_rolls.size() == 2);
            REQUIRE(fixed_rolls.at(0) == 69);
            REQUIRE(fixed_rolls.at(1) == 420);
            const auto& random_rolls = s.get_rolls(IterationType::Random);
            REQUIRE(!random_rolls.empty());
            REQUIRE(random_rolls.size() == 2);
            REQUIRE(random_rolls.at(0) == 420);
            REQUIRE(random_rolls.at(1) == 69);
        }

        SECTION("standard rolls")
        {
            const auto s = Scenario(0,
                                    0,
                                    0,
                                    0,
                                    {},
                                    {},
                                    {},
                                    {},
                                    { 2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12 },
                                    {});
            std::vector<int> expected_rolls;
            std::vector<int> actual_rolls;
            expected_rolls = { 2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12 };
            actual_rolls = s.get_rolls(IterationType::Fixed);
            REQUIRE(!actual_rolls.empty());
            REQUIRE(actual_rolls.size() == expected_rolls.size());
            // NOLINTNEXTLINE(modernize-loop-convert)
            for (int i = 0; i < expected_rolls.size(); ++i) {
                const auto expected_roll = expected_rolls.at(i);
                const auto actual_roll = expected_rolls.at(i);
                REQUIRE(actual_roll == expected_roll);
            }
            expected_rolls = { 3, 5, 11, 2, 5, 8, 9, 9, 10, 4, 10, 12, 3, 4, 8, 11, 6, 6 };
            actual_rolls = s.get_rolls(IterationType::Random);
            REQUIRE(!actual_rolls.empty());
            REQUIRE(actual_rolls.size() == expected_rolls.size());
            // NOLINTNEXTLINE(modernize-loop-convert)
            for (int i = 0; i < expected_rolls.size(); ++i) {
                const auto expected_roll = expected_rolls.at(i);
                const auto actual_roll = expected_rolls.at(i);
                REQUIRE(actual_roll == expected_roll);
            }
        }
    }

    SECTION("get_ports()")
    {
        SECTION("Trivial")
        {
            const auto s = Scenario(0, 0, 0, 0., {}, {}, {}, {}, {}, {});
            REQUIRE(s.get_ports(IterationType::Fixed).empty());
            REQUIRE(s.get_ports(IterationType::Random).empty());
        }

        SECTION("1 port (single resource)")
        {
            const k10engine::ResourceCollection expected_port = { k10engine::Resource::Ore };
            const auto s = Scenario(0, 0, 0, 0, {}, {}, {}, {}, {}, { expected_port });
            const auto& fixed_ports = s.get_ports(IterationType::Fixed);
            REQUIRE(!fixed_ports.empty());
            REQUIRE(fixed_ports.size() == 1);
            REQUIRE(fixed_ports.at(0) == expected_port);
            const auto& random_ports = s.get_ports(IterationType::Random);
            REQUIRE(!random_ports.empty());
            REQUIRE(random_ports.size() == 1);
            REQUIRE(random_ports.at(0) == expected_port);
        }

        SECTION("1 port (multiple resources)")
        {
            const k10engine::ResourceCollection expected_port = { k10engine::Resource::Ore,
                                                                  k10engine::Resource::Wheat };
            const auto s = Scenario(0, 0, 0, 0, {}, {}, {}, {}, {}, { expected_port });
            const auto& fixed_ports = s.get_ports(IterationType::Fixed);
            REQUIRE(!fixed_ports.empty());
            REQUIRE(fixed_ports.size() == 1);
            REQUIRE(fixed_ports.at(0) == expected_port);
            const auto& random_ports = s.get_ports(IterationType::Random);
            REQUIRE(!random_ports.empty());
            REQUIRE(random_ports.size() == 1);
            REQUIRE(random_ports.at(0) == expected_port);
        }

        SECTION("2 port (same, single)")
        {
            k10engine::Random::seed(42);
            const k10engine::ResourceCollection expected_port = { k10engine::Resource::Ore,
                                                                  k10engine::Resource::Wheat };
            const auto s =
                Scenario(0, 0, 0, 0, {}, {}, {}, {}, {}, { expected_port, expected_port });
            const auto& fixed_ports = s.get_ports(IterationType::Fixed);
            REQUIRE(!fixed_ports.empty());
            REQUIRE(fixed_ports.size() == 2);
            REQUIRE(fixed_ports.at(0) == expected_port);
            REQUIRE(fixed_ports.at(1) == expected_port);
            const auto& random_ports = s.get_ports(IterationType::Random);
            REQUIRE(!random_ports.empty());
            REQUIRE(random_ports.size() == 2);
            REQUIRE(random_ports.at(0) == expected_port);
            REQUIRE(random_ports.at(1) == expected_port);
        }

        SECTION("2 port (different, single)")
        {
            k10engine::Random::seed(42);
            std::vector<k10engine::ResourceCollection> expected_ports;
            expected_ports = { { k10engine::Resource::Ore }, { k10engine::Resource::Wheat } };
            const auto s = Scenario(0, 0, 0, 0, {}, {}, {}, {}, {}, expected_ports);
            const auto& fixed_ports = s.get_ports(IterationType::Fixed);
            REQUIRE(!fixed_ports.empty());
            REQUIRE(fixed_ports.size() == expected_ports.size());
            for (int i = 0; i < expected_ports.size(); ++i) {
                REQUIRE(fixed_ports.at(i) == expected_ports.at(i));
            }
            expected_ports = {
                { k10engine::Resource::Wheat },
                { k10engine::Resource::Ore },
            };
            const auto& random_ports = s.get_ports(IterationType::Random);
            REQUIRE(!random_ports.empty());
            REQUIRE(random_ports.size() == expected_ports.size());
            for (int i = 0; i < expected_ports.size(); ++i) {
                REQUIRE(random_ports.at(i) == expected_ports.at(i));
            }
        }

        SECTION("standard ports")
        {
            const std::vector<k10engine::ResourceCollection> ports = {
                { k10engine::Resource::Brick,
                  k10engine::Resource::Ore,
                  k10engine::Resource::Sheep,
                  k10engine::Resource::Wheat,
                  k10engine::Resource::Wood },
                {
                    k10engine::Resource::Wheat,
                },
                {
                    k10engine::Resource::Ore,
                },
                { k10engine::Resource::Brick,
                  k10engine::Resource::Ore,
                  k10engine::Resource::Sheep,
                  k10engine::Resource::Wheat,
                  k10engine::Resource::Wood },
                { k10engine::Resource::Brick,
                  k10engine::Resource::Ore,
                  k10engine::Resource::Sheep,
                  k10engine::Resource::Wheat,
                  k10engine::Resource::Wood },
                {
                    k10engine::Resource::Sheep,
                },
                { k10engine::Resource::Brick,
                  k10engine::Resource::Ore,
                  k10engine::Resource::Sheep,
                  k10engine::Resource::Wheat,
                  k10engine::Resource::Wood },
                {
                    k10engine::Resource::Brick,
                },
                {
                    k10engine::Resource::Wood,
                },
            };
            const auto s = Scenario(0, 0, 0, 0, {}, {}, {}, {}, {}, ports);
            std::vector<k10engine::ResourceCollection> expected_ports;
            std::vector<k10engine::ResourceCollection> actual_ports;
            expected_ports = ports;
            actual_ports = s.get_ports(IterationType::Fixed);
            REQUIRE(!actual_ports.empty());
            REQUIRE(actual_ports.size() == expected_ports.size());
            // NOLINTNEXTLINE(modernize-loop-convert)
            for (int i = 0; i < expected_ports.size(); ++i) {
                const auto expected_port = expected_ports.at(i);
                const auto actual_port = expected_ports.at(i);
                REQUIRE(actual_port == expected_port);
            }
            actual_ports = s.get_ports(IterationType::Random);
            expected_ports = {
                { k10engine::Resource::Brick,
                  k10engine::Resource::Ore,
                  k10engine::Resource::Sheep,
                  k10engine::Resource::Wheat,
                  k10engine::Resource::Wood },
                {
                    k10engine::Resource::Brick,
                },
                {
                    k10engine::Resource::Ore,
                },
                { k10engine::Resource::Brick,
                  k10engine::Resource::Ore,
                  k10engine::Resource::Sheep,
                  k10engine::Resource::Wheat,
                  k10engine::Resource::Wood },
                {
                    k10engine::Resource::Wood,
                },
                {
                    k10engine::Resource::Sheep,
                },
                {
                    k10engine::Resource::Wheat,
                },
                { k10engine::Resource::Brick,
                  k10engine::Resource::Ore,
                  k10engine::Resource::Sheep,
                  k10engine::Resource::Wheat,
                  k10engine::Resource::Wood },
                { k10engine::Resource::Brick,
                  k10engine::Resource::Ore,
                  k10engine::Resource::Sheep,
                  k10engine::Resource::Wheat,
                  k10engine::Resource::Wood },
            };
            REQUIRE(!actual_ports.empty());
            REQUIRE(actual_ports.size() == expected_ports.size());
            // NOLINTNEXTLINE(modernize-loop-convert)
            for (int i = 0; i < expected_ports.size(); ++i) {
                const auto expected_port = expected_ports.at(i);
                const auto actual_port = expected_ports.at(i);
                REQUIRE(actual_port == expected_port);
            }
        }
    }
}

TEST_CASE("Parameter validation", "[Scenario]") {}

} // namespace Scenario

} // namespace k10engine