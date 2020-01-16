#pragma once

#include "Game/BoardView/Hex.h"

namespace k10engine {

namespace Game {

class Robber {
public:
    BoardView::Hex location() { return m_location; }
    void set_location(BoardView::Hex hex) { m_location = hex; }

    Robber(BoardView::Hex hex)
        : m_location(hex)
    {
    }

private:
    BoardView::Hex m_location;
};

} // namespace Game

} // namespace k10engine
