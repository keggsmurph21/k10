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
        serial.push_back(::k10engine::serialize(it.first));
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

    auto num_building_counts = static_cast<size_t>(serial[index++]);
    Counts<Building> building_counts;
    for (size_t i = 0; i < num_building_counts; ++i) {
        auto building = static_cast<Building>(serial[index++]);
        auto count = static_cast<size_t>(serial[index++]);
        building_counts[building] = count;
    }

    auto num_building_counts_per_player = static_cast<size_t>(serial[index++]);
    Counts<Building> building_counts_per_player;
    for (size_t i = 0; i < num_building_counts_per_player; ++i) {
        auto building = static_cast<Building>(serial[index++]);
        auto count = static_cast<size_t>(serial[index++]);
        building_counts_per_player[building] = count;
    }

    auto num_development_card_counts = static_cast<size_t>(serial[index++]);
    Counts<DevelopmentCard> development_card_counts;
    for (size_t i = 0; i < num_development_card_counts; ++i) {
        auto development_card = static_cast<DevelopmentCard>(serial[index++]);
        auto count = static_cast<size_t>(serial[index++]);
        development_card_counts[development_card] = count;
    }

    auto num_resource_counts = static_cast<size_t>(serial[index++]);
    Counts<AbstractResource> resource_counts;
    for (size_t i = 0; i < num_resource_counts; ++i) {
        auto resource = deserialize_abstract_resource(serial[index++]);
        auto count = static_cast<size_t>(serial[index++]);
        resource_counts[resource] = count;
    }

    auto num_rolls = static_cast<size_t>(serial[index++]);
    std::vector<int> rolls;
    for (size_t i = 0; i < num_rolls; ++i) {
        rolls.push_back(static_cast<int>(serial[index++]));
    }

    auto num_port_specs = static_cast<size_t>(serial[index++]);
    std::vector<_PortSpec> port_specs;
    for (size_t i = 0; i < num_port_specs; ++i) {
        auto exchange_rate = static_cast<unsigned>(serial[index++]);
        auto num_resources = static_cast<size_t>(serial[index++]);
        ResourceCollection resources;
        for (size_t j = 0; j < num_resources; ++j) {
            auto resource = static_cast<Resource>(serial[index++]);
            resources.insert(resource);
        }
        _PortSpec port_spec{ resources, exchange_rate };
        port_specs.push_back(port_spec);
    }

    return Scenario(min_players_count,
                    max_players_count,
                    min_victory_points_goal,
                    max_victory_points_goal,
                    std::move(building_costs),
                    std::move(building_counts),
                    std::move(building_counts_per_player),
                    std::move(development_card_counts),
                    std::move(resource_counts),
                    std::move(rolls),
                    std::move(port_specs));
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
            os << ",";
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
            os << ",";
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
            os << ",";
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
            os << ",";
        }
        is_first = false;
        os << " " << it.first << " : " << it.second;
    }
    os << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const _PortSpec& port_spec)
{
    return os << "Port{ "
              << "resources=" << port_spec.resources << ", rate=" << port_spec.exchange_rate
              << " }";
}

std::ostream& operator<<(std::ostream& os, const Scenario& scenario)
{
    os << "Scenario{" << std::endl;
    os << "  min_players_count=" << scenario.m_min_players_count << "," << std::endl;
    os << "  max_players_count=" << scenario.m_max_players_count << "," << std::endl;
    os << "  min_victory_points_goal=" << scenario.m_min_victory_points_goal << "," << std::endl;
    os << "  max_victory_points_goal=" << scenario.m_max_victory_points_goal << "," << std::endl;
    os << "  building_costs=" << scenario.m_building_costs << "," << std::endl;
    os << "  building_counts=" << scenario.m_building_counts << "," << std::endl;
    os << "  building_counts_per_player=" << scenario.m_building_counts_per_player << ","
       << std::endl;
    os << "  development_card_counts=" << scenario.m_development_card_counts << "," << std::endl;
    os << "  resource_counts=" << scenario.m_resource_counts << "," << std::endl;

    os << "  rolls={";
    bool is_first = true;
    for (const auto& roll : scenario.m_rolls) {
        if (!is_first) {
            os << ",";
        }
        is_first = false;
        os << " " << roll;
    }
    os << "}," << std::endl;

    os << "  ports={";
    is_first = true;
    for (const auto& port_spec : scenario.m_ports) {
        if (!is_first) {
            os << ",";
        }
        is_first = false;
        os << " " << port_spec;
    }
    os << "}," << std::endl;

    os << " }";
    return os;
}

bool Scenario::operator==(const Scenario& other) const
{
    if (min_players_count() != other.min_players_count())
        return false;
    if (max_players_count() != other.max_players_count())
        return false;
    if (min_victory_points_goal() != other.min_victory_points_goal())
        return false;
    if (max_victory_points_goal() != other.max_victory_points_goal())
        return false;
    if (building_costs() != other.building_costs())
        return false;
    if (building_counts() != other.building_counts())
        return false;
    if (building_counts_per_player() != other.building_counts_per_player())
        return false;
    if (development_card_counts() != other.development_card_counts())
        return false;
    if (resource_counts() != other.resource_counts())
        return false;
    if (peek_rolls() != other.peek_rolls())
        return false;
    if (peek_ports() != other.peek_ports())
        return false;
    return true;
}

} // namespace k10engine::Scenario
