#pragma once

#include "Scenario/IterationType.h"

namespace k10engine {

namespace Scenario {

typedef struct {
    IterationType development_card_iteration_type;
    IterationType port_iteration_type;
    IterationType resource_iteration_type;
    IterationType roll_iteration_type;
    int players_count;
    int victory_points_goal;
} Parameters;

} // namespace Scenario

} // namespace k10engine
