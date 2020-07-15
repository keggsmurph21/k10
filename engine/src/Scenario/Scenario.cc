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

std::vector<u8> Scenario::get_rolls(IterationType type) const
{
    std::vector<u8> rolls;
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
    return parameters.players_count >= min_players_count() && parameters.players_count <= max_players_count()
           && parameters.victory_points_goal >= min_victory_points_goal()
           && parameters.victory_points_goal <= max_victory_points_goal();
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
              << "resources=" << port_spec.resources << ", rate=" << port_spec.exchange_rate << " }";
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
    os << "  building_counts_per_player=" << scenario.m_building_counts_per_player << "," << std::endl;
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

std::optional<Scenario> Scenario::decode(ByteBuffer& buf)
{
    Decoder decoder(buf);
    size_t min_players_count;
    if (!decoder.decode(min_players_count))
        return {};
    size_t max_players_count;
    if (!decoder.decode(max_players_count))
        return {};
    size_t min_victory_points_goal;
    if (!decoder.decode(min_victory_points_goal))
        return {};
    size_t max_victory_points_goal;
    if (!decoder.decode(max_victory_points_goal))
        return {};
    Costs<Building> building_costs;
    if (!decoder.decode(building_costs))
        return {};
    Counts<Building> building_counts;
    if (!decoder.decode(building_counts))
        return {};
    Counts<Building> building_counts_per_player;
    if (!decoder.decode(building_counts_per_player))
        return {};
    Counts<DevelopmentCard> development_card_counts;
    if (!decoder.decode(development_card_counts))
        return {};
    Counts<AbstractResource> resource_counts;
    if (!decoder.decode(resource_counts))
        return {};
    std::vector<u8> rolls;
    if (!decoder.decode(rolls))
        return {};
    std::vector<_PortSpec> ports;
    if (!decoder.decode(ports))
        return {};
    return Scenario(min_players_count,
                    max_players_count,
                    min_victory_points_goal,
                    max_victory_points_goal,
                    building_costs,
                    building_counts,
                    building_counts_per_player,
                    development_card_counts,
                    resource_counts,
                    rolls,
                    ports);
}

} // namespace k10engine::Scenario

template<typename T>
using Costs = k10engine::Scenario::Costs<T>;

template<typename T>
using Counts = k10engine::Scenario::Counts<T>;

using Building = k10engine::Building;
using DevelopmentCard = k10engine::DevelopmentCard;
using AbstractResource = k10engine::AbstractResource;
using PortSpec = k10engine::Scenario::_PortSpec;
using Scenario = k10engine::Scenario::Scenario;

template<>
void encode(ByteBuffer& buf, const PortSpec& port_spec)
{
    Encoder encoder(buf);
    encoder << port_spec.resources;
    encoder << port_spec.exchange_rate;
}

template<>
bool decode(ByteBuffer& buf, PortSpec& port_spec)
{
    Decoder decoder(buf);
    k10engine::ResourceCollection resources;
    if (!decoder.decode(resources))
        return false;
    port_spec.resources = std::move(resources);
    u8 exchange_rate;
    if (!decoder.decode(exchange_rate))
        return false;
    port_spec.exchange_rate = exchange_rate;
    return true;
}

template<>
void encode(ByteBuffer& buf, const Scenario& scenario)
{
    Encoder encoder(buf);
    encoder << scenario.min_players_count();
    encoder << scenario.max_players_count();
    encoder << scenario.min_victory_points_goal();
    encoder << scenario.max_victory_points_goal();
    encoder << scenario.building_costs();
    encoder << scenario.building_counts();
    encoder << scenario.building_counts_per_player();
    encoder << scenario.development_card_counts();
    encoder << scenario.resource_counts();
    encoder << scenario.peek_rolls();
    encoder << scenario.peek_ports();
}
