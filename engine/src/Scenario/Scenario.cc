#include <algorithm>
#include <assert.h>

#include "Core/Random.h"
#include "Scenario/Scenario.h"

namespace k10engine {

namespace Scenario {

bool Scenario::is_valid(Building b)
{
    return m_building_costs.find(b) != m_building_costs.end();
}

std::optional<ResourceCounts> Scenario::cost(Building)
{
    assert(false);
}

std::optional<int> Scenario::max_count(Building)
{
    assert(false);
}

bool Scenario::is_valid(DevelopmentCard d)
{
    return m_development_card_counts.find(d) != m_development_card_counts.end();
}

std::optional<int> Scenario::count(DevelopmentCard)
{
    assert(false);
}

bool Scenario::is_valid(AbstractResource r)
{
    return m_resource_counts.find(r) != m_resource_counts.end();
}

std::vector<DevelopmentCard> Scenario::get_development_card_deck(IterationType type)
{
    std::vector<DevelopmentCard> deck;
    for (const auto& item : m_development_card_counts) {
        auto development_card = item.first;
        auto count = item.second;
        for (int i = 0; i < count; ++i)
            deck.push_back(development_card);
    }
    if (type == IterationType::Fixed) {
        return deck;
    } else if (type == IterationType::Random) {
        std::shuffle(deck.begin(), deck.end(), Random::rng());
        return deck;
    } else {
        assert(false);
    }
}

std::vector<AbstractResource> Scenario::get_resources(IterationType type)
{
    std::vector<AbstractResource> resources;
    for (const auto& item : m_resource_counts) {
        auto resource = item.first;
        auto count = item.second;
        for (int i = 0; i < count; ++i)
            resources.push_back(resource);
    }
    if (type == IterationType::Fixed) {
        return resources;
    } else if (type == IterationType::Random) {
        std::shuffle(resources.begin(), resources.end(), Random::rng());
        return resources;
    } else {
        assert(false);
    }
}

std::vector<ResourceCollection> Scenario::get_ports(IterationType type)
{
    std::vector<ResourceCollection> ports(m_ports.size());
    for (const auto& port_type : m_ports)
        ports.push_back(port_type);
    if (type == IterationType::Fixed) {
        return ports;
    } else if (type == IterationType::Random) {
        std::shuffle(ports.begin(), ports.end(), Random::rng());
        return ports;
    } else {
        assert(false);
    }
}

std::vector<int> Scenario::get_rolls(IterationType type)
{
    std::vector<int> rolls(m_rolls.size());
    for (const auto& roll : m_rolls)
        rolls.push_back(roll);
    if (type == IterationType::Fixed) {
        return rolls;
    } else if (type == IterationType::Random) {
        std::shuffle(rolls.begin(), rolls.end(), Random::rng());
        return rolls;
    } else {
        assert(false);
    }
}

Scenario::~Scenario()
{
    m_building_costs.clear();
    m_building_max_counts.clear();
    m_development_card_counts.clear();
    m_resource_counts.clear();
    m_ports.clear();
}

bool Scenario::is_valid(Parameters* parameters)
{
    return parameters->players_count >= min_players_count()
           && parameters->players_count <= max_players_count()
           && parameters->victory_points_goal >= min_victory_points_goal()
           && parameters->victory_points_goal <= max_victory_points_goal();
}

} // namespace Scenario

} // namespace k10engine
