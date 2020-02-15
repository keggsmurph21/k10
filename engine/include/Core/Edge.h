#pragma once

#include <iostream>

namespace k10engine::State {

enum class Edge {
    AcceptTrade,
    Build,
    CancelTrade,
    CollectResources,
    DeclineTrade,
    Discard,
    FailTradeUnableToFindPartner,
    MoveRobber,
    OfferTrade,
    PlayDevelopmentCard,
    RollDice,
    Steal,
    ToRoot,
};

std::ostream& operator<<(std::ostream&, Edge);

} // namespace k10engine::State
