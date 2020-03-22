#pragma once

#include <iostream>
#include <vector>

#include "Core/Resource.h"

namespace k10engine::Game {

class Player;

struct Trade {
    const Player* offerer;
    const std::vector<Player*> offered_to;
    const ResourceCounts from_offerer;
    const ResourceCounts to_offerer;
};

std::ostream& operator<<(std::ostream&, const Trade&);

} // namespace k10engine::Game
