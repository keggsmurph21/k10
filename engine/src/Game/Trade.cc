#include "Game/Trade.h"
#include "Game/Player.h"

namespace k10engine::Game {

std::ostream& operator<<(std::ostream& os, const Trade& trade)
{
    os << "Trade{ " << *trade.offerer << ", with: { ";
    bool is_first = true;
    for (const auto& offered_to : trade.offered_to) {
        if (!is_first) {
            os << ", ";
        }
        os << *offered_to;
        is_first = false;
    }
    os << " }, from: " << trade.from_offerer;
    os << ", to: " << trade.to_offerer;
    os << "}";
    return os;
}

} // namespace k10engine::Game
