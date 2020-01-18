#pragma once

#include <map>
#include <optional>
#include <set>
#include <vector>

#include "Core/Building.h"
#include "Core/DevelopmentCard.h"
#include "Core/Resource.h"
#include "Scenario/IterationType.h"
#include "Scenario/Parameters.h"

namespace k10engine {

namespace Scenario {

template<typename T>
using Costs = std::map<T, ResourceCounts, std::less<T>>;

template<typename T>
using Counts = std::map<T, int, std::less<T>>;

class Scenario {
public:
    int min_players_count() { return m_min_players_count; }
    int max_players_count() { return m_max_players_count; }
    int min_victory_points_goal() { return m_min_victory_points_goal; }
    int max_victory_points_goal() { return m_max_victory_points_goal; }

    bool is_valid(Building);
    Costs<Building>* building_costs() { return &m_building_costs; }
    std::optional<ResourceCounts> cost(Building);
    Counts<Building>* building_max_counts() { return &m_building_max_counts; }
    std::optional<int> max_count(Building);

    bool is_valid(DevelopmentCard);
    Counts<DevelopmentCard>* development_card_counts() { return &m_development_card_counts; }
    std::optional<int> count(DevelopmentCard);

    bool is_valid(AbstractResource);
    Counts<AbstractResource>* resource_counts() { return &m_resource_counts; }
    std::optional<int> count(AbstractResource);

    std::vector<DevelopmentCard> get_development_card_deck(IterationType); // gets a fresh copy
    std::vector<ResourceCollection> get_ports(IterationType);              // gets a fresh copy
    std::vector<AbstractResource> get_resources(IterationType);            // gets a fresh copy
    std::vector<int> get_rolls(IterationType);                             // gets a fresh copy

    bool is_valid(Parameters*);

    Scenario(int min_players_count,
             int max_players_count,
             int min_victory_points_goal,
             int max_victory_points_goal,
             Costs<Building> building_costs,
             Counts<Building> building_max_counts,
             Counts<DevelopmentCard> development_card_counts,
             Counts<AbstractResource> resource_counts,
             std::vector<int> rolls,
             std::vector<ResourceCollection> ports)
        : m_min_players_count(min_players_count)
        , m_max_players_count(max_players_count)
        , m_min_victory_points_goal(min_victory_points_goal)
        , m_max_victory_points_goal(max_victory_points_goal)
        , m_building_costs(building_costs)
        , m_building_max_counts(building_max_counts)
        , m_development_card_counts(development_card_counts)
        , m_resource_counts(resource_counts)
        , m_rolls(rolls)
        , m_ports(ports)
    {
    }
    ~Scenario();

protected:
    int m_min_players_count = -1;
    int m_max_players_count = -1;
    int m_min_victory_points_goal = -1;
    int m_max_victory_points_goal = -1;

    Costs<Building> m_building_costs;
    Counts<Building> m_building_max_counts;
    Counts<DevelopmentCard> m_development_card_counts;
    Counts<AbstractResource> m_resource_counts;
    std::vector<int> m_rolls;

    std::vector<ResourceCollection> m_ports;
};

} // namespace Scenario

} // namespace k10engine
