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

namespace k10engine::Scenario {

template<typename T>
using Costs = std::map<T, ResourceCounts, std::less<T>>;

template<typename T>
using Counts = std::map<T, int, std::less<T>>;

class Scenario {
public:
    int min_players_count() const { return m_min_players_count; }
    int max_players_count() const { return m_max_players_count; }
    int min_victory_points_goal() const { return m_min_victory_points_goal; }
    int max_victory_points_goal() const { return m_max_victory_points_goal; }

    const Costs<Building>& building_costs() const { return m_building_costs; }
    const Counts<Building>& building_counts() const { return m_building_counts; }
    const Counts<AbstractResource>& resource_counts() const { return m_resource_counts; }
    const Counts<DevelopmentCard>& development_card_counts() const
    {
        return m_development_card_counts;
    }

    bool is_valid(const Building&) const;
    const ResourceCounts* cost(const Building&) const;
    int count(const Building&) const;

    bool is_valid(const DevelopmentCard&) const;
    int count(const DevelopmentCard&) const;

    bool is_valid(const AbstractResource&) const;
    int count(const AbstractResource&) const;

    std::vector<const DevelopmentCard*>
        get_development_card_deck(IterationType) const;                      // gets a fresh copy
    std::vector<const ResourceCollection*> get_ports(IterationType) const;   // gets a fresh copy
    std::vector<const AbstractResource*> get_resources(IterationType) const; // gets a fresh copy
    std::vector<int> get_rolls(IterationType) const;                         // gets a fresh copy

    bool is_valid(Parameters*) const;

    Scenario(int min_players_count,
             int max_players_count,
             int min_victory_points_goal,
             int max_victory_points_goal,
             Costs<Building> building_costs,
             Counts<Building> building_counts,
             Counts<DevelopmentCard> development_card_counts,
             Counts<AbstractResource> resource_counts,
             std::vector<int> rolls,
             std::vector<ResourceCollection> ports)
        : m_min_players_count(min_players_count)
        , m_max_players_count(max_players_count)
        , m_min_victory_points_goal(min_victory_points_goal)
        , m_max_victory_points_goal(max_victory_points_goal)
        , m_building_costs(building_costs)
        , m_building_counts(building_counts)
        , m_development_card_counts(development_card_counts)
        , m_resource_counts(resource_counts)
        , m_rolls(rolls)
        , m_ports(ports)
    {
    }
    ~Scenario();

protected:
    int m_min_players_count;
    int m_max_players_count;
    int m_min_victory_points_goal;
    int m_max_victory_points_goal;

    Costs<Building> m_building_costs;
    Counts<Building> m_building_counts;
    Counts<DevelopmentCard> m_development_card_counts;
    Counts<AbstractResource> m_resource_counts;
    std::vector<int> m_rolls;

    std::vector<ResourceCollection> m_ports;
};

} // namespace k10engine::Scenario
