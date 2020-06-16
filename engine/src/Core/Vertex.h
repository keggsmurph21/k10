#pragma once

#include <iostream>

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
