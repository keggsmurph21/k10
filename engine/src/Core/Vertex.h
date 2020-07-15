#pragma once

#include <iostream>

#include "Util/Decoder.h"
#include "Util/Encoder.h"

namespace k10engine::State {

enum class Vertex {
    AfterBuildingFreeSettlement,
    AfterDiscarding,
    AfterMovingRobber,
    AfterRollingSeven,
    ChooseInitialResources,
    GameOver,
    Root,
    WaitForTurn,
    WaitForTradeResponses,
};

std::ostream& operator<<(std::ostream&, Vertex);

} // namespace k10engine::State

template<>
void encode(ByteBuffer&, const k10engine::State::Vertex&);

template<>
bool decode(ByteBuffer&, k10engine::State::Vertex&);
