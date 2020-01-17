#pragma once

#include <map>
#include <set>
#include <string.h>
#include <vector>

#include "Scenario/Base.h"

namespace k10engine {

namespace Scenario {

class Standard : public Base {
public:
    Standard();

private:
    static const int s_min_players_count = 2;
    static const int s_max_players_count = 10;
    static const int s_min_victory_points_goal = 6;
    static const int s_max_victory_points_goal = 12;
};

} // namespace Scenario

} // namespace k10engine
