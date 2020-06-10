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

} // namespace k10engine::Game

using ActionArgumentType = k10engine::Game::ActionArgumentType;

template<>
void encode(ByteBuffer& buf, const ActionArgumentType& type)
{
    Encoder encoder(buf);
    encoder << static_cast<u8>(type);
}

template<>
bool decode(ByteBuffer& buf, ActionArgumentType& type)
{
    Decoder decoder(buf);
    u8 byte;
    if (!decoder.decode(byte))
        return false;

    switch (static_cast<ActionArgumentType>(byte)) {
    case ActionArgumentType::NodeId:
        type = ActionArgumentType::NodeId;
        break;
    case ActionArgumentType::DiceRoll:
        type = ActionArgumentType::DiceRoll;
        break;
    case ActionArgumentType::PlayerId:
        type = ActionArgumentType::PlayerId;
        break;
    case ActionArgumentType::BuildItemId:
        type = ActionArgumentType::BuildItemId;
        break;
    case ActionArgumentType::DevelopmentCardId:
        type = ActionArgumentType::DevelopmentCardId;
        break;
    case ActionArgumentType::TakeResourceType:
        type = ActionArgumentType::TakeResourceType;
        break;
    case ActionArgumentType::GiveResourceType:
        type = ActionArgumentType::GiveResourceType;
        break;
    case ActionArgumentType::ResourceCount:
        type = ActionArgumentType::ResourceCount;
        break;
    default:
        return false;
    }
    return true;
}

using ActionArgument = k10engine::Game::ActionArgument;

template<>
void encode(ByteBuffer& buf, const ActionArgument& action_argument)
{
    Encoder encoder(buf);
    encoder << action_argument.type;
    encoder << action_argument.value;
}

template<>
bool decode(ByteBuffer& buf, ActionArgument& action_argument)
{
    Decoder decoder(buf);
    ActionArgumentType type;
    if (!decoder.decode(type))
        return false;

    size_t value;
    if (!decoder.decode(value))
        return false;

    action_argument = { type, value };
    return true;
}

using Action = k10engine::Game::Action;

template<>
void encode(ByteBuffer& buf, const Action& action)
{
    Encoder encoder(buf);
    encoder << action.edge;
    encoder << action.args;
}

template<>
bool decode(ByteBuffer& buf, Action& action)
{
    Decoder decoder(buf);
    k10engine::State::Edge edge;
    if (!decoder.decode(edge))
        return false;

    std::vector<ActionArgument> args;
    if (!decoder.decode(args))
        return false;

    action = { edge, std::move(args) };
    return true;
}
