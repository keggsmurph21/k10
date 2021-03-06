#pragma once

#include <algorithm>
#include <map>
#include <optional>
#include <set>
#include <vector>

#include "Board/Graph.h"
#include "Core/Building.h"
#include "Core/DevelopmentCard.h"
#include "Core/Resource.h"
#include "Scenario/IterationType.h"
#include "Scenario/Parameters.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"

namespace k10engine::Scenario {

#define k10_SCENARIO_MIN_PLAYERS_COUNT 1
#define k10_SCENARIO_MAX_PLAYERS_COUNT 10
#define k10_SCENARIO_MIN_VICTORY_POINTS_GOAL 2
#define k10_SCENARIO_MAX_VICTORY_POINTS_GOAL 20

template<typename T>
using Costs = std::map<T, ResourceCounts, std::less<T>>;
std::ostream& operator<<(std::ostream&, const Costs<Building>&);

template<typename T>
using Counts = std::map<T, size_t, std::less<T>>;
std::ostream& operator<<(std::ostream&, const Counts<Building>&);
std::ostream& operator<<(std::ostream&, const Counts<DevelopmentCard>&);
std::ostream& operator<<(std::ostream&, const Counts<AbstractResource>&);

struct _PortSpec {
    ResourceCollection resources;
    u8 exchange_rate;
    bool operator==(const _PortSpec& other) const
    {
        return resources == other.resources && exchange_rate == other.exchange_rate;
    }
    bool operator!=(const _PortSpec& other) const { return !(*this == other); }
};
std::ostream& operator<<(std::ostream&, const _PortSpec&);

class Scenario {
public:
    const Board::Graph& graph() const { return m_graph; }

    size_t min_players_count() const { return m_min_players_count; }
    size_t max_players_count() const { return m_max_players_count; }
    size_t min_victory_points_goal() const { return m_min_victory_points_goal; }
    size_t max_victory_points_goal() const { return m_max_victory_points_goal; }

    const Costs<Building>& building_costs() const { return m_building_costs; }
    const Counts<Building>& building_counts() const { return m_building_counts; }
    const Counts<Building>& building_counts_per_player() const { return m_building_counts_per_player; }
    const Counts<AbstractResource>& resource_counts() const { return m_resource_counts; }
    const Counts<DevelopmentCard>& development_card_counts() const { return m_development_card_counts; }

    bool is_valid(const Building&) const;
    const ResourceCounts* cost(const Building&) const;
    size_t count(const Building&) const;
    size_t count_per_player(const Building&) const;

    bool is_valid(const DevelopmentCard&) const;
    size_t count(const DevelopmentCard&) const;

    bool is_valid(const AbstractResource&) const;
    size_t count(const AbstractResource&) const;

    std::vector<DevelopmentCard> get_development_card_deck(IterationType) const; // gets a fresh copy
    std::vector<_PortSpec> get_ports(IterationType) const;                       // gets a fresh copy
    std::vector<AbstractResource> get_resources(IterationType) const;            // gets a fresh copy
    std::vector<u8> get_rolls(IterationType) const;                              // gets a fresh copy

    const std::vector<u8>& peek_rolls() const { return m_rolls; }
    const std::vector<_PortSpec>& peek_ports() const { return m_ports; }

    bool is_valid(const Parameters&) const;

    friend std::ostream& operator<<(std::ostream&, const Scenario&);

    Scenario(const Board::Graph& graph,
             size_t min_players_count,
             size_t max_players_count,
             size_t min_victory_points_goal,
             size_t max_victory_points_goal,
             Costs<Building> building_costs,
             Counts<Building> building_counts,
             Counts<Building> building_counts_per_player,
             Counts<DevelopmentCard> development_card_counts,
             Counts<AbstractResource> resource_counts,
             std::vector<u8> rolls,
             std::vector<_PortSpec> ports)
        : m_graph(graph)
        , m_building_costs(building_costs)
        , m_building_counts(building_counts)
        , m_building_counts_per_player(building_counts_per_player)
        , m_development_card_counts(development_card_counts)
        , m_resource_counts(resource_counts)
        , m_rolls(rolls)
        , m_ports(ports)
    {
        m_min_players_count = std::max<size_t>(min_players_count, k10_SCENARIO_MIN_PLAYERS_COUNT);
        m_max_players_count = std::min<size_t>(max_players_count, k10_SCENARIO_MAX_PLAYERS_COUNT);
        m_min_victory_points_goal = std::max<size_t>(min_victory_points_goal, k10_SCENARIO_MIN_VICTORY_POINTS_GOAL);
        m_max_victory_points_goal = std::min<size_t>(max_victory_points_goal, k10_SCENARIO_MAX_VICTORY_POINTS_GOAL);
    }
    ~Scenario();

    bool operator==(const Scenario&) const;

protected:
    const Board::Graph& m_graph;

    size_t m_min_players_count;
    size_t m_max_players_count;
    size_t m_min_victory_points_goal;
    size_t m_max_victory_points_goal;

    Costs<Building> m_building_costs;
    Counts<Building> m_building_counts;
    Counts<Building> m_building_counts_per_player;
    Counts<DevelopmentCard> m_development_card_counts;
    Counts<AbstractResource> m_resource_counts;
    std::vector<u8> m_rolls;

    std::vector<_PortSpec> m_ports;
};

} // namespace k10engine::Scenario

template<>
void encode(ByteBuffer&, const k10engine::Scenario::_PortSpec&);

template<>
bool decode(ByteBuffer&, k10engine::Scenario::_PortSpec&);

template<>
void encode(ByteBuffer&, const k10engine::Scenario::Scenario* const&);

template<>
bool decode(ByteBuffer&, const k10engine::Scenario::Scenario*&);
