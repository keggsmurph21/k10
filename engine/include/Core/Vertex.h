#pragma once

#include <iostream>

namespace k10engine::State {

enum class Vertex {
    AcceptTrade,
    AcceptTradeOther,
    Build, // City, DevelopmentCard, Road, Settlement
    Collect,
    Discard,
    DiscardOther,
    EndGame,
    EndTurn,
    InitCollect,
    MoveRobber,
    OfferTrade,
    PlayDevelopmentCard, // Knight, Monopoly, RoadBuilding, VictoryPoint, YearOfPlenty
    Roll,
    Root,
    Steal,
    TradeWithBank,
};

std::ostream& operator<<(std::ostream&, Vertex);

} // namespace k10engine::State
