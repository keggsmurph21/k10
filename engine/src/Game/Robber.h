#pragma once

#include "Game/BoardView/Hex.h"

namespace k10engine {

namespace Game {

class Robber {
public:
    const BoardView::Hex* location() const { return m_location; }
    void set_location(const BoardView::Hex* location) { m_location = location; }
    Robber(const BoardView::Hex* location)
        : m_location(location)
    {
    }

private:
    const BoardView::Hex* m_location{ nullptr };
};

} // namespace Game

} // namespace k10engine
