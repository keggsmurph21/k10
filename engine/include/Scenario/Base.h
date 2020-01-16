#pragma once

#include <map>
#include <optional>
#include <set>
#include <vector>

#include "Core/Building.h"
#include "Core/DevelopmentCard.h"
#include "Core/Resource.h"

namespace k10engine {

namespace Scenario {

enum class IterationType {
    Fixed,
    Random,
};

template<typename T>
using Costs = std::map<T, ResourceCounts, std::less<T>>;

template<typename T>
using Counts = std::map<T, int, std::less<T>>;

class Base {
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
    std::vector<AbstractResource> get_resources(IterationType);            // gets a fresh copy
    std::vector<ResourceCollection> get_ports(IterationType);              // gets a fresh copy

    Base() {}
    ~Base();

protected:
    int m_min_players_count;
    int m_max_players_count;
    int m_min_victory_points_goal;
    int m_max_victory_points_goal;

    Costs<Building> m_building_costs;
    Counts<Building> m_building_max_counts;
    Counts<DevelopmentCard> m_development_card_counts;
    Counts<AbstractResource> m_resource_counts;

    std::vector<ResourceCollection> m_ports;
};

} // namespace Scenario

} // namespace k10engine
