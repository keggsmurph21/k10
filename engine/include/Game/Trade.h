#pragma once

#include <iostream>
#include <vector>

#include "Core/Resource.h"

namespace k10engine::Game {

class Player;

struct Trade {
    Player* offerer;
    std::vector<Player*> offered_to;
    ResourceCounts from_offerer;
    ResourceCounts to_offerer;
};

std::ostream& operator<<(std::ostream&, const Trade&);

} // namespace k10engine::Game
