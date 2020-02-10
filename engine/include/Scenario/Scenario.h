#pragma once

#include <algorithm>
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

#define k10_SCENARIO_MIN_PLAYERS_COUNT 1
#define k10_SCENARIO_MAX_PLAYERS_COUNT 10
#define k10_SCENARIO_MIN_VICTORY_POINTS_GOAL 2
#define k10_SCENARIO_MAX_VICTORY_POINTS_GOAL 20

template<typename T>
using Costs = std::map<T, ResourceCounts, std::less<T>>;

template<typename T>
using Counts = std::map<T, int, std::less<T>>;

struct _PortSpec {
    ResourceCollection resources;
    unsigned exchange_rate;
    bool operator==(const _PortSpec other) const
    {
        return resources == other.resources && exchange_rate == other.exchange_rate;
    }
};

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

    std::vector<DevelopmentCard>
        get_development_card_deck(IterationType) const;               // gets a fresh copy
    std::vector<_PortSpec> get_ports(IterationType) const;            // gets a fresh copy
    std::vector<AbstractResource> get_resources(IterationType) const; // gets a fresh copy
    std::vector<int> get_rolls(IterationType) const;                  // gets a fresh copy

    bool is_valid(const Parameters&) const;

    Scenario(int min_players_count,
             int max_players_count,
             int min_victory_points_goal,
             int max_victory_points_goal,
             Costs<Building> building_costs,
             Counts<Building> building_counts,
             Counts<DevelopmentCard> development_card_counts,
             Counts<AbstractResource> resource_counts,
             std::vector<int> rolls,
             std::vector<_PortSpec> ports)
        : m_building_costs(building_costs)
        , m_building_counts(building_counts)
        , m_development_card_counts(development_card_counts)
        , m_resource_counts(resource_counts)
        , m_rolls(rolls)
        , m_ports(ports)
    {
        m_min_players_count = std::max<int>(min_players_count, k10_SCENARIO_MIN_PLAYERS_COUNT);
        m_max_players_count = std::min<int>(max_players_count, k10_SCENARIO_MAX_PLAYERS_COUNT);
        m_min_victory_points_goal =
            std::max<int>(min_victory_points_goal, k10_SCENARIO_MIN_VICTORY_POINTS_GOAL);
        m_max_victory_points_goal =
            std::min<int>(max_victory_points_goal, k10_SCENARIO_MAX_VICTORY_POINTS_GOAL);
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

    std::vector<_PortSpec> m_ports;
};

} // namespace k10engine::Scenario
