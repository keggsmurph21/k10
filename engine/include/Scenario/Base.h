#pragma once

#include <list>
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

class Base {
public:
    static int min_players_count() { return s_min_players_count; }
    static int max_players_count() { return s_max_players_count; }
    static int min_victory_points_goal() { return s_min_victory_points_goal; }
    static int max_victory_points_goal() { return s_max_victory_points_goal; }

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

    std::list<DevelopmentCard> get_development_card_deck(IterationType); // gets a fresh copy
    std::vector<ResourceCollection> get_ports(IterationType);            // gets a fresh copy
    std::vector<AbstractResource> get_resources(IterationType);          // gets a fresh copy
    std::vector<int> get_rolls(IterationType);                           // gets a fresh copy

    static bool is_valid(Parameters*);

    Base() {}
    ~Base();

protected:
    static const int s_min_players_count = -1;
    static const int s_max_players_count = -1;
    static const int s_min_victory_points_goal = -1;
    static const int s_max_victory_points_goal = -1;

    Costs<Building> m_building_costs;
    Counts<Building> m_building_max_counts;
    Counts<DevelopmentCard> m_development_card_counts;
    Counts<AbstractResource> m_resource_counts;
    std::vector<int> m_rolls;

    std::vector<ResourceCollection> m_ports;
};

} // namespace Scenario

} // namespace k10engine
