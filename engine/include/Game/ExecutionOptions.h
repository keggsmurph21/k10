#pragma once

namespace k10engine::Game {

enum class Options : unsigned {
    None = 1 << 0,
    NoCost = 1 << 1,
};

Options operator&(Options l_opt, Options r_opt);

} // namespace k10engine::Game
