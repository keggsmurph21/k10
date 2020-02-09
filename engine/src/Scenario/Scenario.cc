#include <algorithm>
#include <stdexcept>

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

int Scenario::count(const Building& building) const
{
    if (!is_valid(building)) {
        return 0;
    }
    return m_building_counts.at(building);
}

bool Scenario::is_valid(const DevelopmentCard& development_card) const
{
    return m_development_card_counts.find(development_card) != m_development_card_counts.end();
}

int Scenario::count(const DevelopmentCard& development_card) const
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

int Scenario::count(const AbstractResource& resource) const
{
    if (!is_valid(resource)) {
        return 0;
    }
    return m_resource_counts.at(resource);
}

const std::vector<DevelopmentCard> Scenario::get_development_card_deck(IterationType type) const
{
    std::vector<DevelopmentCard> deck;
    for (const auto& item : m_development_card_counts) {
        auto development_card = item.first;
        auto count = item.second;
        for (int i = 0; i < count; ++i) {
            deck.push_back(development_card);
        }
    }
    switch (type) {
    case IterationType::Fixed:
        return deck;
    case IterationType::Random:
        std::shuffle(deck.begin(), deck.end(), Random::rng());
        return deck;
    default:
        throw std::invalid_argument("Unrecognized IterationType");
    }
}

const std::vector<AbstractResource> Scenario::get_resources(IterationType type) const
{
    std::vector<AbstractResource> resources;
    for (const auto& item : m_resource_counts) {
        auto resource = item.first;
        auto count = item.second;
        for (int i = 0; i < count; ++i) {
            resources.push_back(resource);
        }
    }
    switch (type) {
    case IterationType::Fixed:
        return resources;
    case IterationType::Random:
        std::shuffle(resources.begin(), resources.end(), Random::rng());
        return resources;
    default:
        throw std::invalid_argument("Unrecognized IterationType");
    }
}

const std::vector<ResourceCollection> Scenario::get_ports(IterationType type) const
{
    std::vector<ResourceCollection> ports;
    for (const auto& port_type : m_ports) {
        ports.push_back(port_type);
    }
    switch (type) {
    case IterationType::Fixed:
        return ports;
    case IterationType::Random:
        std::shuffle(ports.begin(), ports.end(), Random::rng());
        return ports;
    default:
        throw std::invalid_argument("Unrecognized IterationType");
    }
}

const std::vector<int> Scenario::get_rolls(IterationType type) const
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
    default:
        throw std::invalid_argument("Unrecognized IterationType");
    }
}

Scenario::~Scenario()
{
    m_building_costs.clear();
    m_building_counts.clear();
    m_development_card_counts.clear();
    m_resource_counts.clear();
    m_ports.clear();
}

bool Scenario::is_valid(Parameters* parameters) const
{
    return parameters->players_count >= min_players_count()
           && parameters->players_count <= max_players_count()
           && parameters->victory_points_goal >= min_victory_points_goal()
           && parameters->victory_points_goal <= max_victory_points_goal();
}

} // namespace k10engine::Scenario
