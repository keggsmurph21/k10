#pragma once

#include <iostream>

#include "Util/ByteBuffer.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"

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

template<>
void encode(ByteBuffer&, const k10engine::State::Edge&);

template<>
bool decode(ByteBuffer&, k10engine::State::Edge&);
