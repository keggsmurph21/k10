#include <assert.h>

#include "Scenario/Base.h"

namespace k10engine {

namespace Scenario {

bool Base::is_valid(Building b)
{
    return m_building_costs.find(b) != m_building_costs.end();
}

std::optional<ResourceCounts> Base::cost(Building)
{
    assert(false);
}

std::optional<int> Base::max_count(Building)
{
    assert(false);
}

bool Base::is_valid(DevelopmentCard d)
{
    return m_development_card_counts.find(d) != m_development_card_counts.end();
}

std::optional<int> Base::count(DevelopmentCard)
{
    assert(false);
}

bool Base::is_valid(AbstractResource r)
{
    return m_resource_counts.find(r) != m_resource_counts.end();
}

std::list<DevelopmentCard> Base::get_development_card_deck(IterationType type)
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

std::vector<AbstractResource> Base::get_resources(IterationType)
{
    assert(false);
}

std::vector<ResourceCollection> Base::get_ports(IterationType)
{
    assert(false);
}

std::vector<int> Base::get_rolls(IterationType)
{
    assert(false);
}

Base::~Base()
{
    m_building_costs.clear();
    m_building_max_counts.clear();
    m_development_card_counts.clear();
    m_resource_counts.clear();
    m_ports.clear();
}

bool Base::is_valid(Parameters* parameters)
{
    return parameters->players_count >= min_players_count()
           && parameters->players_count <= max_players_count()
           && parameters->victory_points_goal >= min_victory_points_goal()
           && parameters->victory_points_goal <= max_victory_points_goal();
}

} // namespace Scenario

} // namespace k10engine
