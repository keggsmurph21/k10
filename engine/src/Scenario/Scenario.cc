#include <assert.h>

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

std::list<DevelopmentCard> Scenario::get_development_card_deck(IterationType type)
{
    assert(false);
    (void)type;
    std::list<DevelopmentCard> deck;
    for (const auto& item : m_development_card_counts) {
        auto development_card = item.first;
        auto count = item.second;
        for (int i = 0; i < count; ++i)
            deck.push_back(development_card);
    }
    return deck;
}

std::vector<AbstractResource> Scenario::get_resources(IterationType)
{
    assert(false);
}

std::vector<ResourceCollection> Scenario::get_ports(IterationType)
{
    assert(false);
}

std::vector<int> Scenario::get_rolls(IterationType)
{
    assert(false);
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
