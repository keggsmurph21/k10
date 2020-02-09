#include <iostream>

#include "Board/generated/Standard.h"
#include "Game/Game.h"
#include "Scenario/Scenario.h"

int main(int /* unused */, char** /* unused */)
{
    auto b = k10engine::Board::get_standard_board();
    auto s = k10engine::Scenario::Scenario(2, 2, 2, 2, {}, {}, {}, {}, {}, {});
    auto p = k10engine::Scenario::Parameters{ k10engine::Scenario::IterationType::Fixed,
                                              k10engine::Scenario::IterationType::Fixed,
                                              k10engine::Scenario::IterationType::Fixed,
                                              k10engine::Scenario::IterationType::Fixed,
                                              2,
                                              2 };
    auto g = k10engine::Game::initialize(b, s, p);
    (void)g;

    delete b;

    return 0;
}
