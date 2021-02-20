#pragma once

#include <vector>

#include "Board/Graph.h"
#include "Util/LRUCache.h"

namespace k10engine::Board {

enum class Name {
    Portless,
    Single,
    Standard,
    Tall,
    Triple,
};

class Store {
public:
    static const Store& the();
    ~Store();

    const Board::Graph* by_name(const Name&) const;
    Name name_of(const Board::Graph*) const;

private:
    Store();
    std::vector<const Board::Graph*> m_named_boards;
};

} // namespace k10engine::Board
