#pragma once

#include <iostream>

namespace k10engine::State {

enum class Edge {
    AcceptTrade,
    AcceptTradeOther,
    AfterDiscardOther,
    AfterTradeOther,
    Build, // City, DevelopmentCard, Road, Settlement
    CancelTrade,
    DeclineTrade,
    DiscardMoveRobber,
    EndGame,
    EndInit,
    EndTurn,
    FailTrade,
    InitBuildFirstRoad,
    InitBuildSecondRoad,
    InitCollect,
    InitSettle,
    NoStealRobber,
    OfferTrade,
    PlayDevelopmentCard, // Knight, Monopoly, RoadBuilding, VictoryPoint, YearOfPlenty
    Roll,
    RollCollect,
    RollDiscard,
    RollDiscardOther,
    RollMoveRobber,
    StealRobber,
    TakeTurn,
    ToRoot,
    TradeBank,
};

std::ostream& operator<<(std::ostream&, Edge);

} // namespace k10engine::State
