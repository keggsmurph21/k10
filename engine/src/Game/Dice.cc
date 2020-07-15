#include <cassert>

#include "Core/Random.h"
#include "Game/Dice.h"

static auto s_dice_dist = std::uniform_int_distribution<u8>(k10_DICE_MIN, k10_DICE_MAX);

namespace k10engine::Game {

void Dice::roll()
{
    auto rng = Random::rng();
    m_die_0 = s_dice_dist(rng);
    m_die_1 = s_dice_dist(rng);
}

u8 Dice::get_total() const
{
    return m_die_0 + m_die_1;
}

#ifdef k10_ENABLE_ROLL_DICE_EXACT
void Dice::set_total(u8 total)
{
    assert(1 < total && total < 13);
    if (total > 6) {
        m_die_0 = 6;
        m_die_1 = total - 6;
    } else {
        m_die_0 = 1;
        m_die_1 = total - 1;
    }
}
#endif

} // namespace k10engine::Game

using Dice = k10engine::Game::Dice;

template<>
void encode(ByteBuffer& buf, const Dice& dice)
{
    Encoder encoder(buf);
    encoder << dice.die_0();
    encoder << dice.die_1();
}

template<>
bool decode(ByteBuffer& buf, k10engine::Game::Dice& dice)
{
    Decoder decoder(buf);
    u8 die_0;
    if (!decoder.decode(die_0))
        return false;
    u8 die_1;
    if (!decoder.decode(die_1))
        return false;
    dice.set_die_0(die_0);
    dice.set_die_1(die_1);
    return true;
}
