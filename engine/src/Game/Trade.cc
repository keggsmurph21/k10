#include <cassert>

#include "Game/Player.h"
#include "Game/Trade.h"

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

bool Trade::operator==(const Trade& that) const
{
    assert(this->offerer != nullptr);
    assert(that.offerer != nullptr);
    if (this->offerer->index() != that.offerer->index())
        return false;
    if (this->offered_to.size() != that.offered_to.size())
        return false;
    for (size_t i = 0; i < this->offered_to.size(); ++i) {
        assert(this->offered_to.at(i) != nullptr);
        assert(that.offered_to.at(i) != nullptr);
        if (this->offered_to.at(i)->index() != that.offered_to.at(i)->index())
            return false;
    }
    if (this->from_offerer != that.from_offerer)
        return false;
    return this->to_offerer == that.to_offerer;
}

} // namespace k10engine::Game
