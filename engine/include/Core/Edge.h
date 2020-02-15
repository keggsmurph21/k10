#pragma once

#include <iostream>

namespace k10engine::State {

enum class Edge {
    AcceptTrade,
    AcceptTradeOther,
    AfterTradeOther,
    Build, // City, DevelopmentCard, Road, Settlement
    CancelTrade,
    CollectResources,
    DeclineTrade,
    Discard,
    EndGame,
    EndInit,
    FailTradeUnableToFindPartner,
    InitBuildFirstRoad,
    InitBuildSecondRoad,
    InitCollectResources,
    InitSettle,
    MoveRobber,
    OfferTrade,
    PlayDevelopmentCard, // Knight, Monopoly, RoadBuilding, VictoryPoint, YearOfPlenty
    RollDice,
    Steal,
    TakeTurn,
    ToRoot,
    TradeBank,
};

std::ostream& operator<<(std::ostream&, Edge);

} // namespace k10engine::State
