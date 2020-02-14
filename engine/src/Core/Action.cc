#include "Core/Action.h"
#include "Core/Building.h"
#include "Core/DevelopmentCard.h"

namespace k10engine::Game {

std::ostream& operator<<(std::ostream& os, const ActionArgumentType& type)
{
    switch (type) {
    case ActionArgumentType::NodeId:
        os << "Type::NodeId";
        break;
    case ActionArgumentType::DiceRoll:
        os << "Type::DiceRoll";
        break;
    case ActionArgumentType::PlayerId:
        os << "Type::PlayerId";
        break;
    case ActionArgumentType::BuildItemId:
        os << "Type::BuildItemId";
        break;
    case ActionArgumentType::DevelopmentCardId:
        os << "Type::DevelopmentCardId";
        break;
    case ActionArgumentType::TakeResourceType:
        os << "Type::TakeResourceType";
        break;
    case ActionArgumentType::GiveResourceType:
        os << "Type::GiveResourceType";
        break;
    case ActionArgumentType::ResourceCount:
        os << "Type::ResourceCount";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const ActionArgument& arg)
{
    os << "{ " << arg.type << ", ";
    switch (arg.type) {
    case ActionArgumentType::BuildItemId:
        os << static_cast<Building>(arg.value);
        break;
    case ActionArgumentType::DevelopmentCardId:
        os << static_cast<DevelopmentCard>(arg.value);
        break;
    case ActionArgumentType::NodeId:
    case ActionArgumentType::PlayerId:
    case ActionArgumentType::TakeResourceType:
    case ActionArgumentType::GiveResourceType:
    case ActionArgumentType::DiceRoll:
    case ActionArgumentType::ResourceCount:
        os << arg.value;
        break;
    }
    os << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Action& action)
{
    os << "Action{ " << action.edge << ", { ";
    bool is_first = true;
    for (const auto& arg : action.args) {
        if (!is_first) {
            os << ", ";
        }
        os << arg;
        is_first = false;
    }
    os << " } }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Result& result)
{
    if (result.was_valid) {
        os << "Result{ ";
        if (!result.args.empty()) {
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
    } else {
        os << "InvalidResult{ messages{ ";
        bool is_first = true;
        for (const auto& msg : result.messages) {
            if (!is_first) {
                os << ", ";
            }
            os << msg;
            is_first = false;
        }
    }
    return os;
}

} // namespace k10engine::Game
