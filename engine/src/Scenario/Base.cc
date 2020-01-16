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

std::vector<DevelopmentCard> Base::get_development_card_deck(IterationType type)
{
    assert(false);
    (void)type;
    std::vector<DevelopmentCard> deck;
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

Base::~Base()
{
    m_building_costs.clear();
    m_building_max_counts.clear();
    m_development_card_counts.clear();
    m_resource_counts.clear();
    m_ports.clear();
}

} // namespace Scenario

} // namespace k10engine
