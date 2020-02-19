#pragma once

#include <iostream>

#include "Core/Action.h"

namespace k10engine::Game {

enum class ResultType {
    Ok,
    InvalidPlayerId,
    InvalidEdgeChoice,
    InvalidNumberOfArgs,
    InvalidArgumentType,
    BuildingIdOutOfRange,
    NodeIdOutOfRange,
    InvalidNodeId,
    JunctionNotSettleable,
    CannotAfford,
    DiceRollOutOfRange,
};

struct Result {
    ResultType type{ ResultType::Ok };
    std::vector<ActionArgument> args; // only (?) for ::RollDice
};

std::ostream& operator<<(std::ostream&, const ResultType&);
std::ostream& operator<<(std::ostream&, const Result&);

} // namespace k10engine::Game
