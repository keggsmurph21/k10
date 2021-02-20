#include "Scenario/Parameters.h"

using IterationType = k10engine::Scenario::IterationType;

template<>
bool decode(ByteBuffer& buf, IterationType& iteration_type)
{
    Decoder decoder(buf);

    u8 byte;
    if (!decoder.decode(byte))
        return false;

    switch (static_cast<IterationType>(byte)) {
    case IterationType::Fixed:
        iteration_type = IterationType::Fixed;
        return true;
    case IterationType::Random:
        iteration_type = IterationType::Random;
        return true;
    default:
        return false;
    }
}

template<>
bool decode(ByteBuffer& buf, k10engine::Scenario::Parameters& parameters)
{
    Decoder decoder(buf);

    IterationType development_card_iteration_type;
    if (!decoder.decode(development_card_iteration_type))
        return false;

    IterationType port_iteration_type;
    if (!decoder.decode(port_iteration_type))
        return false;

    IterationType resource_iteration_type;
    if (!decoder.decode(resource_iteration_type))
        return false;

    IterationType roll_iteration_type;
    if (!decoder.decode(roll_iteration_type))
        return false;

    size_t players_count;
    if (!decoder.decode(players_count))
        return false;

    size_t victory_points_goal;
    if (!decoder.decode(victory_points_goal))
        return false;

    parameters = { development_card_iteration_type,
                   port_iteration_type,
                   resource_iteration_type,
                   roll_iteration_type,
                   players_count,
                   victory_points_goal };
    return true;
}
