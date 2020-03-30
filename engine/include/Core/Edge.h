#pragma once

#include <iostream>

namespace k10engine::State {

enum class Edge {
    AcceptTrade,
    BuildCity,
    BuildDevelopmentCard,
    BuildRoad,
    BuildSettlement,
    CancelTrade,
    ChooseInitialResources,
    DeclineTrade,
    Discard,
    EndTurn,
    FailTradeUnableToFindPartner,
    MoveRobber,
    OfferTrade,
    PlayKnight,
    PlayMonopoly,
    PlayRoadBuilding,
    PlayVictoryPoint,
    PlayYearOfPlenty,
    RollDice,
    Steal,
    ToRoot,
    TradeWithBank,
};

std::ostream& operator<<(std::ostream&, Edge);

} // namespace k10engine::State
