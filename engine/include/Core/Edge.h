#pragma once

#include <iostream>

namespace k10engine::State {

enum class Edge {
    AcceptTrade,
    AcceptTradeOther,
    AfterTradeOther,
    Build, // City, DevelopmentCard, Road, Settlement
    CancelTrade,
    DeclineTrade,
    EndGame,
    EndInit,
    EndTurn,
    FailTradeUnableToFindPartner,
    InitBuildFirstRoad,
    InitBuildSecondRoad,
    InitCollectResources,
    InitSettle,
    NoStealRobber,
    OfferTrade,
    PlayDevelopmentCard, // Knight, Monopoly, RoadBuilding, VictoryPoint, YearOfPlenty
    RollDice,
    CollectResources,
    Discard,
    MoveRobber,
    StealRobber,
    TakeTurn,
    ToRoot,
    TradeBank,
};

std::ostream& operator<<(std::ostream&, Edge);

} // namespace k10engine::State
