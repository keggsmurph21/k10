#include "Core/Result.h"

namespace k10engine::Game {

std::ostream& operator<<(std::ostream& os, const ResultType& error)
{
    switch (error) {
    case ResultType::Ok:
        os << "ResultType::Ok";
        break;
    case ResultType::InvalidPlayerId:
        os << "ResultType::InvalidPlayerId";
        break;
    case ResultType::InvalidEdgeChoice:
        os << "ResultType::InvalidEdgeChoice";
        break;
    case ResultType::InvalidNumberOfArgs:
        os << "ResultType::InvalidNumberOfArgs";
        break;
    case ResultType::InvalidArgumentType:
        os << "ResultType::InvalidArgumentType";
        break;
    case ResultType::BuildingIdOutOfRange:
        os << "ResultType::BuildingIdOutOfRange";
        break;
    case ResultType::NodeIdOutOfRange:
        os << "ResultType::NodeIdOutOfRange";
        break;
    case ResultType::InvalidNodeId:
        os << "ResultType::InvalidNodeType";
        break;
    case ResultType::JunctionNotSettleable:
        os << "ResultType::JunctionNotSettleable";
        break;
    case ResultType::CannotAfford:
        os << "ResultType::CannotAfford";
        break;
    case ResultType::DiceRollOutOfRange:
        os << "ResultType::DiceRollOutOfRange";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Result& result)
{
    os << "Result{ " << result.type;
    if (!result.args.empty()) {
        os << " { ";
        bool is_first = true;
        for (const auto& arg : result.args) {
            if (!is_first) {
                os << ", ";
            }
            os << arg;
            is_first = false;
        }
        os << " }";
    }
    os << " }";
    return os;
}

} // namespace k10engine::Game
