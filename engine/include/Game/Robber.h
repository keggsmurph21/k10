#pragma once

#include "Game/BoardView/Hex.h"

namespace k10engine {

namespace Game {

class Robber {
public:
    const BoardView::Hex* location() { return m_location; }
    void set_location(const BoardView::Hex* hex) { m_location = hex; }

    Robber() {}

private:
    const BoardView::Hex* m_location{ nullptr };
};

} // namespace Game

} // namespace k10engine
