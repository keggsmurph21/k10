#pragma once

#include "Scenario/IterationType.h"
#include "Util/ByteBuffer.h"
#include "Util/Decoder.h"

namespace k10engine::Scenario {

struct Parameters {
    IterationType development_card_iteration_type;
    IterationType port_iteration_type;
    IterationType resource_iteration_type;
    IterationType roll_iteration_type;
    // FIXME: These can be u8s :^)
    size_t players_count;
    size_t victory_points_goal;
};

} // namespace k10engine::Scenario

template<>
bool decode(ByteBuffer&, k10engine::Scenario::IterationType&);

template<>
bool decode(ByteBuffer&, k10engine::Scenario::Parameters&);
