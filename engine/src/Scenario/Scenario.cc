#include <algorithm>
#include <cassert>

#include "Core/Random.h"
#include "Scenario/Scenario.h"

namespace k10engine::Scenario {

bool Scenario::is_valid(const Building& building) const
{
    return m_building_costs.find(building) != m_building_costs.end();
}

const ResourceCounts* Scenario::cost(const Building& building) const
{
    if (!is_valid(building)) {
        return nullptr;
    }
    return &m_building_costs.at(building);
}

size_t Scenario::count(const Building& building) const
{
    if (m_building_counts.find(building) == m_building_counts.end()) {
        return 0;
    }
    return m_building_counts.at(building);
}

size_t Scenario::count_per_player(const Building& building) const
{
    if (m_building_counts_per_player.find(building) == m_building_counts_per_player.end()) {
        return 0;
    }
    return m_building_counts_per_player.at(building);
}

bool Scenario::is_valid(const DevelopmentCard& development_card) const
{
    return m_development_card_counts.find(development_card) != m_development_card_counts.end();
}

size_t Scenario::count(const DevelopmentCard& development_card) const
{
    if (!is_valid(development_card)) {
        return 0;
    }
    return m_development_card_counts.at(development_card);
}

bool Scenario::is_valid(const AbstractResource& resource) const
{
    return m_resource_counts.find(resource) != m_resource_counts.end();
}

size_t Scenario::count(const AbstractResource& resource) const
{
    if (!is_valid(resource)) {
        return 0;
    }
    return m_resource_counts.at(resource);
}

std::vector<DevelopmentCard> Scenario::get_development_card_deck(IterationType type) const
{
    std::vector<DevelopmentCard> deck;
    for (const auto& item : m_development_card_counts) {
        auto count = item.second;
        for (size_t i = 0; i < count; ++i) {
            deck.push_back(item.first);
        }
    }
    switch (type) {
    case IterationType::Fixed:
        return deck;
    case IterationType::Random:
        std::shuffle(deck.begin(), deck.end(), Random::rng());
        return deck;
    }
    assert(false);
}

std::vector<AbstractResource> Scenario::get_resources(IterationType type) const
{
    std::vector<AbstractResource> resources;
    for (const auto& item : m_resource_counts) {
        auto count = item.second;
        for (size_t i = 0; i < count; ++i) {
            resources.push_back(item.first);
        }
    }
    switch (type) {
    case IterationType::Fixed:
        return resources;
    case IterationType::Random:
        std::shuffle(resources.begin(), resources.end(), Random::rng());
        return resources;
    }
    assert(false);
}

std::vector<_PortSpec> Scenario::get_ports(IterationType type) const
{
    std::vector<_PortSpec> ports;
    for (const auto& port_spec : m_ports) {
        ports.push_back(port_spec);
    }
    switch (type) {
    case IterationType::Fixed:
        return ports;
    case IterationType::Random:
        std::shuffle(ports.begin(), ports.end(), Random::rng());
        return ports;
    }
    assert(false);
}

std::vector<int> Scenario::get_rolls(IterationType type) const
{
    std::vector<int> rolls;
    for (const auto& roll : m_rolls) {
        rolls.push_back(roll);
    }
    switch (type) {
    case IterationType::Fixed:
        return rolls;
    case IterationType::Random:
        std::shuffle(rolls.begin(), rolls.end(), Random::rng());
        return rolls;
    }
    assert(false);
}

Scenario::~Scenario()
{
    m_building_costs.clear();
    m_building_counts.clear();
    m_development_card_counts.clear();
    m_resource_counts.clear();
    m_ports.clear();
}

bool Scenario::is_valid(const Parameters& parameters) const
{
    return parameters.players_count >= min_players_count()
           && parameters.players_count <= max_players_count()
           && parameters.victory_points_goal >= min_victory_points_goal()
           && parameters.victory_points_goal <= max_victory_points_goal();
}

std::vector<u8> Scenario::serialize() const
{
    std::vector<u8> serial;
    serial.push_back(static_cast<u8>(m_min_players_count));
    serial.push_back(static_cast<u8>(m_max_players_count));
    serial.push_back(static_cast<u8>(m_min_victory_points_goal));
    serial.push_back(static_cast<u8>(m_max_victory_points_goal));

    std::cout << m_building_costs << std::endl;
    serial.push_back(static_cast<u8>(m_building_costs.size()));
    for (const auto& it : m_building_costs) {
        serial.push_back(static_cast<u8>(it.first));
        serial.push_back(static_cast<u8>(it.second.size()));
        for (const auto& jt : it.second) {
            serial.push_back(static_cast<u8>(jt.first));
            serial.push_back(static_cast<u8>(jt.second));
        }
    }

    serial.push_back(static_cast<u8>(m_building_counts.size()));
    for (const auto& it : m_building_counts) {
        serial.push_back(static_cast<u8>(it.first));
        serial.push_back(static_cast<u8>(it.second));
    }

    serial.push_back(static_cast<u8>(m_building_counts_per_player.size()));
    for (const auto& it : m_building_counts_per_player) {
        serial.push_back(static_cast<u8>(it.first));
        serial.push_back(static_cast<u8>(it.second));
    }

    serial.push_back(static_cast<u8>(m_development_card_counts.size()));
    for (const auto& it : m_development_card_counts) {
        serial.push_back(static_cast<u8>(it.first));
        serial.push_back(static_cast<u8>(it.second));
    }

    serial.push_back(static_cast<u8>(m_resource_counts.size()));
    for (const auto& it : m_resource_counts) {
        if (std::holds_alternative<Resource>(it.first)) {
            serial.push_back(static_cast<u8>(std::get<Resource>(it.first)));
        } else {
            serial.push_back(static_cast<u8>(std::get<NonYieldingResource>(it.first)) + 0xf0);
        }
        serial.push_back(static_cast<u8>(it.second));
    }

    serial.push_back(static_cast<u8>(m_rolls.size()));
    for (const auto& roll : m_rolls) {
        serial.push_back(static_cast<u8>(roll));
    }

    serial.push_back(static_cast<u8>(m_ports.size()));
    for (const auto& port_spec : m_ports) {
        serial.push_back(static_cast<u8>(port_spec.exchange_rate));
        serial.push_back(static_cast<u8>(port_spec.resources.size()));
        for (const auto& resource : port_spec.resources) {
            serial.push_back(static_cast<u8>(resource));
        }
    }

    return serial;
}

Scenario Scenario::deserialize(const std::vector<u8>& serial)
{
    size_t index = 0;

    auto min_players_count = static_cast<size_t>(serial[index++]);
    auto max_players_count = static_cast<size_t>(serial[index++]);
    auto min_victory_points_goal = static_cast<size_t>(serial[index++]);
    auto max_victory_points_goal = static_cast<size_t>(serial[index++]);

    auto num_building_costs = static_cast<size_t>(serial[index++]);
    Costs<Building> building_costs;
    for (size_t i = 0; i < num_building_costs; ++i) {
        auto building = static_cast<Building>(serial[index++]);
        auto num_resources = static_cast<size_t>(serial[index++]);
        ResourceCounts cost;
        for (size_t j = 0; j < num_resources; ++j) {
            auto resource = static_cast<Resource>(serial[index++]);
            auto count = static_cast<size_t>(serial[index++]);
            cost[resource] = count;
        }
        building_costs[building] = cost;
    }
    std::cout << building_costs << std::endl;

    (void)min_players_count;
    (void)max_players_count;
    (void)min_victory_points_goal;
    (void)max_victory_points_goal;
    /*
    Scenario(size_t min_players_count,
             size_t max_players_count,
             size_t min_victory_points_goal,
             size_t max_victory_points_goal,
             Costs<Building> building_costs,
             Counts<Building> building_counts,
             Counts<Building> building_counts_per_player,
             Counts<DevelopmentCard> development_card_counts,
             Counts<AbstractResource> resource_counts,
             std::vector<int> rolls,
             std::vector<_PortSpec> ports)
             */
    assert(false);
}

std::ostream& operator<<(std::ostream& os, const Costs<Building>& building_costs)
{
    os << "BuildingCosts{";
    if (building_costs.empty()) {
        os << "}";
        return os;
    }
    bool is_first = true;
    for (const auto& it : building_costs) {
        if (!is_first) {
            os << " ";
        }
        is_first = false;
        os << " " << it.first << " : " << it.second;
    }
    os << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Counts<Building>& building_counts)
{
    os << "BuildingCounts{";
    if (building_counts.empty()) {
        os << "}";
        return os;
    }
    bool is_first = true;
    for (const auto& it : building_counts) {
        if (!is_first) {
            os << " ";
        }
        is_first = false;
        os << " " << it.first << " : " << it.second;
    }
    os << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Counts<DevelopmentCard>& development_card_counts)
{
    os << "DevelopmentCardCounts{";
    if (development_card_counts.empty()) {
        os << "}";
        return os;
    }
    bool is_first = true;
    for (const auto& it : development_card_counts) {
        if (!is_first) {
            os << " ";
        }
        is_first = false;
        os << " " << it.first << " : " << it.second;
    }
    os << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Counts<AbstractResource>& resource_counts)
{
    os << "ResourceCounts{";
    if (resource_counts.empty()) {
        os << "}";
        return os;
    }
    bool is_first = true;
    for (const auto& it : resource_counts) {
        if (!is_first) {
            os << " ";
        }
        is_first = false;
        os << " " << it.first << " : " << it.second;
    }
    os << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Scenario& scenario)
{
    os << "Scenario{ ";
    os << "min_players_count=" << scenario.m_min_players_count << ", ";
    os << "max_players_count=" << scenario.m_max_players_count << ", ";
    os << "min_victory_points_goal=" << scenario.m_min_victory_points_goal << ", ";
    os << "max_victory_points_goal=" << scenario.m_max_victory_points_goal << ", ";
    os << "building_costs=" << scenario.m_building_costs << ", ";
    os << "building_costs=" << scenario.m_building_costs << ", ";
    os << "building_costs=" << scenario.m_building_costs << ", ";
    os << "building_costs=" << scenario.m_building_costs << ", ";
    os << "building_costs=" << scenario.m_building_costs << ", ";
    /*
    Scenario(size_t min_players_count,
             size_t max_players_count,
             size_t min_victory_points_goal,
             size_t max_victory_points_goal,
             Costs<Building> building_costs,
             Counts<Building> building_counts,
             Counts<Building> building_counts_per_player,
             Counts<DevelopmentCard> development_card_counts,
             Counts<AbstractResource> resource_counts,
             std::vector<int> rolls,
             std::vector<_PortSpec> ports)
             */
    return os;
}

} // namespace k10engine::Scenario
