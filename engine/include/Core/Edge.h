#pragma once

#include <iostream>

namespace k10engine::State {

enum class Edge {
    AcceptTrade,
    Build,
    CancelTrade,
    ChooseInitialResources,
    CollectResources,
    DeclineTrade,
    Discard,
    EndTurn,
    FailTradeUnableToFindPartner,
    MoveRobber,
    OfferTrade,
    PlayDevelopmentCard,
    RollDice,
    Steal,
    ToRoot,
    TradeWithBank,
};

std::ostream& operator<<(std::ostream&, Edge);

} // namespace k10engine::State
