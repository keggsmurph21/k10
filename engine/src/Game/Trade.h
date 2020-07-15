#pragma once

#include <iostream>
#include <vector>

#include "Core/Resource.h"
#include "Forward.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"

namespace k10engine::Game {

struct Trade {
    Player* offerer;
    std::vector<Player*> offered_to;
    ResourceCounts from_offerer;
    ResourceCounts to_offerer;
    bool operator==(const Trade&) const;
};

std::ostream& operator<<(std::ostream&, const Trade&);

} // namespace k10engine::Game
