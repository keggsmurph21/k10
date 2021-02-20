#include "Game/ExecutionOptions.h"

namespace k10engine::Game {

Options operator&(Options l_opt, Options r_opt)
{
    return static_cast<Options>(static_cast<unsigned>(l_opt) & static_cast<unsigned>(r_opt));
}

} // namespace k10engine::Game
