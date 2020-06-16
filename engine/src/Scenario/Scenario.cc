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

#define PLAYERS_CAN_RANGE_FROM(min, max) (min), (max)
#define VICTORY_POINTS_CAN_RANGE_FROM(min, max) (min), (max)
#define TO_BUILD(...) \
    {                 \
        __VA_ARGS__   \
    }
#define A(building, ...)                    \
    {                                       \
        Building::building, { __VA_ARGS__ } \
    }
#define IT
#define COSTS(num, resource)    \
    {                           \
        Resource::resource, num \
    }
#define AND ,
#define THERE_CAN_BE(...) \
    {                     \
        __VA_ARGS__       \
    }
#define EACH_PLAYER_CAN_HAVE(...) \
    {                             \
        __VA_ARGS__               \
    }
#define AT_MOST(num, building)  \
    {                           \
        Building::building, num \
    }
#define THE_DECK_CONTAINS(...) \
    {                          \
        __VA_ARGS__            \
    }
#define EXACTLY(num, development_card)         \
    {                                          \
        DevelopmentCard::development_card, num \
    }
#define THE_ROLLS_ARE(...) \
    {                      \
        __VA_ARGS__        \
    }
#define THE_RESOURCES_ARE(...) \
    {                          \
        __VA_ARGS__            \
    }
#define THE_PORTS_ARE(...) \
    {                      \
        __VA_ARGS__        \
    }
#define EXCHANGE_ANYTHING_FOR(num, of_x) \
    {                                    \
        of_x, num                        \
    }
#define OF(resource)       \
    {                      \
        Resource::resource \
    }
#define OF_ANYTHING                                                                      \
    {                                                                                    \
        Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood \
    }

const Scenario* get_single_scenario()
{
    static auto* single = new Scenario{
        PLAYERS_CAN_RANGE_FROM(1, 2),
        VICTORY_POINTS_CAN_RANGE_FROM(3, 6),
        TO_BUILD(A(City, IT COSTS(3, Ore) AND COSTS(2, Wheat)),
                 A(DevelopmentCard, IT COSTS(1, Ore) AND COSTS(1, Sheep) AND COSTS(1, Wheat)),
                 A(Road, IT COSTS(1, Brick) AND COSTS(1, Wood)),
                 A(Settlement, IT COSTS(1, Brick) AND COSTS(1, Sheep) AND COSTS(1, Wheat) AND COSTS(1, Wood))),
        THERE_CAN_BE(AT_MOST(25, DevelopmentCard)),
        EACH_PLAYER_CAN_HAVE(AT_MOST(2, City), AT_MOST(6, Road), AT_MOST(2, Settlement)),
        THE_DECK_CONTAINS(EXACTLY(14, Knight),
                          EXACTLY(2, Monopoly),
                          EXACTLY(2, RoadBuilding),
                          EXACTLY(5, VictoryPoint),
                          EXACTLY(2, YearOfPlenty)),
        THE_RESOURCES_ARE({ NonYieldingResource::Desert, 1 }),
        THE_ROLLS_ARE(),
        THE_PORTS_ARE(EXCHANGE_ANYTHING_FOR(3, OF_ANYTHING), EXCHANGE_ANYTHING_FOR(3, OF(Wheat)))
    };
    return single;
}

const Scenario* get_triple_scenario()
{
    static auto* triple = new Scenario{
        PLAYERS_CAN_RANGE_FROM(1, 3),
        VICTORY_POINTS_CAN_RANGE_FROM(3, 6),
        TO_BUILD(A(City, IT COSTS(3, Ore) AND COSTS(2, Wheat)),
                 A(DevelopmentCard, IT COSTS(1, Ore) AND COSTS(1, Sheep) AND COSTS(1, Wheat)),
                 A(Road, IT COSTS(1, Brick) AND COSTS(1, Wood)),
                 A(Settlement, IT COSTS(1, Brick) AND COSTS(1, Sheep) AND COSTS(1, Wheat) AND COSTS(1, Wood))),
        THERE_CAN_BE(AT_MOST(25, DevelopmentCard)),
        EACH_PLAYER_CAN_HAVE(AT_MOST(3, City), AT_MOST(10, Road), AT_MOST(4, Settlement)),
        THE_DECK_CONTAINS(EXACTLY(14, Knight),
                          EXACTLY(2, Monopoly),
                          EXACTLY(2, RoadBuilding),
                          EXACTLY(5, VictoryPoint),
                          EXACTLY(2, YearOfPlenty)),
        THE_RESOURCES_ARE({ Resource::Brick, 1 }, { Resource::Wood, 1 }, { NonYieldingResource::Desert, 1 }),
        THE_ROLLS_ARE(6, 9),
        THE_PORTS_ARE(EXCHANGE_ANYTHING_FOR(2, OF(Brick)),
                      EXCHANGE_ANYTHING_FOR(2, OF(Wood)),
                      EXCHANGE_ANYTHING_FOR(2, OF(Wheat)),
                      EXCHANGE_ANYTHING_FOR(3, OF_ANYTHING))
    };
    return triple;
}

const Scenario* get_standard_scenario()
{
    static auto* standard = new Scenario{
        PLAYERS_CAN_RANGE_FROM(2, 5),
        VICTORY_POINTS_CAN_RANGE_FROM(8, 12),
        TO_BUILD(A(City, IT COSTS(3, Ore) AND COSTS(2, Wheat)),
                 A(DevelopmentCard, IT COSTS(1, Ore) AND COSTS(1, Sheep) AND COSTS(1, Wheat)),
                 A(Road, IT COSTS(1, Brick) AND COSTS(1, Wood)),
                 A(Settlement, IT COSTS(1, Brick) AND COSTS(1, Sheep) AND COSTS(1, Wheat) AND COSTS(1, Wood))),
        THERE_CAN_BE(AT_MOST(25, DevelopmentCard)),
        EACH_PLAYER_CAN_HAVE(AT_MOST(4, City), AT_MOST(15, Road), AT_MOST(5, Settlement)),
        THE_DECK_CONTAINS(EXACTLY(14, Knight),
                          EXACTLY(2, Monopoly),
                          EXACTLY(2, RoadBuilding),
                          EXACTLY(5, VictoryPoint),
                          EXACTLY(2, YearOfPlenty)),
        THE_RESOURCES_ARE({ Resource::Brick, 3 },
                          { Resource::Ore, 3 },
                          { Resource::Sheep, 4 },
                          { Resource::Wheat, 4 },
                          { Resource::Wood, 4 },
                          { NonYieldingResource::Desert, 1 }),
        THE_ROLLS_ARE(2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12),
        THE_PORTS_ARE(EXCHANGE_ANYTHING_FOR(3, OF_ANYTHING),
                      EXCHANGE_ANYTHING_FOR(2, OF(Wheat)),
                      EXCHANGE_ANYTHING_FOR(2, OF(Ore)),
                      EXCHANGE_ANYTHING_FOR(3, OF_ANYTHING),
                      EXCHANGE_ANYTHING_FOR(3, OF_ANYTHING),
                      EXCHANGE_ANYTHING_FOR(2, OF(Sheep)),
                      EXCHANGE_ANYTHING_FOR(3, OF_ANYTHING),
                      EXCHANGE_ANYTHING_FOR(2, OF(Brick)),
                      EXCHANGE_ANYTHING_FOR(2, OF(Wood)))
    };
    return standard;
}

} // namespace k10engine::Scenario
