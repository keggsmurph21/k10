#include <cassert>

#include "Game/View.h"

namespace k10engine::Game {

#define DO_BASIC_INIT_FROM_GAME()                                       \
    m_scenario_name = Scenario::Store::the().name_of(&game.scenario()); \
    m_num_players = game.players().size();                              \
    m_victory_points_goal = game.victory_points_goal();

// Type::NotStarted
View::View(const Game& game, PlayerId player_id, std::vector<PlayerId> joined_player_ids)
    : m_type(Type::NotStarted)
    , m_game(game)
    , m_player_id(player_id)
    , m_joined_player_ids(std::move(joined_player_ids))
{
    DO_BASIC_INIT_FROM_GAME();
}

// Type::NotInGame
View::View(const Game& game, PlayerId player_id)
    : m_type(Type::NotStarted)
    , m_game(game)
    , m_player_id(player_id)
    , m_turn(game.turn())
{
    DO_BASIC_INIT_FROM_GAME();
}

// Type::InGame
View::View(const Game& game, PlayerId player_id, View::Dice dice, size_t remaining_development_cards)
    : m_type(Type::InGame)
    , m_game(game)
    , m_player_id(player_id)
    , m_dice(dice)
    , m_remaining_development_cards(remaining_development_cards)
{
    DO_BASIC_INIT_FROM_GAME();
    m_robber_hex_index = m_game.robber_location()->index();
    m_current_player_index = m_game.current_player().index();
    m_has_rolled = m_game.has_rolled();
    // FIXME: Deref the pointers inside current_trade() at this point
    m_current_trade = m_game.current_trade();
    if (m_game.has_largest_army())
        m_has_largest_army = m_game.has_largest_army()->id();
    if (m_game.has_longest_road())
        m_has_longest_road = m_game.has_longest_road()->id();
}

void View::encode(ByteBuffer& buf) const
{
    Encoder encoder(buf);

    encoder << m_type;
    encoder << m_scenario_name;
    encoder << m_num_players;
    encoder << m_victory_points_goal;

    if (m_type == View::Type::NotStarted) {
        encoder << m_joined_player_ids;
        return;
    }

    encoder << m_turn;

    if (m_type == View::Type::NotInGame)
        return;

    // Type::InGame
    encoder << m_dice.die_0;
    encoder << m_dice.die_1;
    encoder << m_remaining_development_cards;
    encoder << m_robber_hex_index;
    encoder << m_current_player_index;
    encoder << m_has_rolled;
    encoder << m_current_trade;
    encoder << m_has_largest_army;
    encoder << m_has_longest_road;
}

} // namespace k10engine::Game

using View = k10engine::Game::View;

template<>
void encode(ByteBuffer& buf, const View& view)
{
    view.encode(buf);
}

template<>
void encode(ByteBuffer& buf, const View::Type& view)
{
    Encoder encoder(buf);
    encoder << static_cast<u8>(view);
}
