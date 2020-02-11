#pragma once

#include <iostream>

namespace k10engine::State {

enum class Vertex {
    AfterBuilding,
    AfterDiscarding,
    AfterMovingRobber,
    AfterRoll,
    GameOver,
    Root,
    WaitForTurn,
    WaitingForTradeResponses,
};

std::ostream& operator<<(std::ostream&, Vertex);

} // namespace k10engine::State
