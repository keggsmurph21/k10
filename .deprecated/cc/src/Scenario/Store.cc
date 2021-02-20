#include <cassert>

#include "Board/Store.h"
#include "Scenario/Store.h"

namespace k10engine::Scenario {

static Store* s_scenario_store = nullptr;

const Store& Store::the()
{
    if (s_scenario_store == nullptr)
        s_scenario_store = new Store();
    return *s_scenario_store;
}

// Dumb little DSL for constructing these scenarios ... it'd be much better to
// just eventually move this into an actual config file format :^)

#define WITH_BOARD(x) (*Board::Store::the().by_name(Board::Name::x))
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

Store::Store()
{
    m_named_scenarios.resize(5);
    m_named_scenarios.at(static_cast<size_t>(Name::Single)) = new Scenario{
        WITH_BOARD(Single),
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
    m_named_scenarios.at(static_cast<size_t>(Name::Triple)) = new Scenario{
        WITH_BOARD(Triple),
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
    m_named_scenarios.at(static_cast<size_t>(Name::Standard)) = new Scenario{
        WITH_BOARD(Standard),
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
}

Store::~Store()
{
    for (auto* named_scenario : m_named_scenarios)
        delete named_scenario;
}

const Scenario* Store::by_name(const Name& name) const
{
    switch (name) {
    case Name::Single:
    case Name::Triple:
    case Name::Standard:
        return m_named_scenarios[static_cast<size_t>(name)];
    default:
        return nullptr;
    }
}

Name Store::name_of(const Scenario* scenario) const
{
    for (size_t i = 0; i < m_named_scenarios.size(); ++i) {
        const auto* named_scenario = m_named_scenarios.at(i);
        if (named_scenario == scenario)
            return static_cast<Name>(i);
    }
    assert(false);
}

} // namespace k10engine::Scenario

using Name = k10engine::Scenario::Name;

template<>
void encode(ByteBuffer& buf, const Name& name)
{
    Encoder encoder(buf);
    encoder << static_cast<u8>(name);
}

template<>
bool decode(ByteBuffer& buf, Name& name)
{
    Decoder decoder(buf);

    u8 byte;
    if (!decoder.decode(byte))
        return false;

    name = static_cast<Name>(byte);
    return true;
}
