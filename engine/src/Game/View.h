#pragma once

#include "Game/Game.h"
#include "Scenario/Store.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"

namespace k10engine::Game {

class View {
public:
    enum class Type {
        NotStarted,
        NotInGame,
        InGame,
    };

    struct Dice {
        u8 die_0;
        u8 die_1;
    };

    View(const Game&, PlayerId, std::vector<PlayerId> joined_player_ids);
    View(const Game&, PlayerId);
    View(const Game&, PlayerId, Dice, size_t remaining_development_cards);

    void encode(ByteBuffer&) const;

private:
    // Type::NotStarted && Type::NotInGame && Type::InGame
    const Type m_type;
    const Game& m_game;
    const PlayerId m_player_id;
    Scenario::Name m_scenario_name;
    size_t m_num_players;
    size_t m_victory_points_goal;

    // Type::NotStarted
    std::vector<PlayerId> m_joined_player_ids;

    // Type::NotInGame && Type::InGame
    size_t m_turn{ 0 };

    // Type::InGame
    Dice m_dice{ 0, 0 };
    size_t m_remaining_development_cards{ 0 };
    size_t m_robber_hex_index{ 0 };
    size_t m_current_player_index{ 0 };
    bool m_has_rolled{ false };
    std::optional<Trade> m_current_trade;
    std::optional<PlayerId> m_has_largest_army;
    std::optional<PlayerId> m_has_longest_road;

    // FIXME: Maybe these would be useful later?
    // bool m_can_steal;
    // bool m_is_trade_accepted;
    // size_t m_num_trades_offered_this_turn;
};

} // namespace k10engine::Game

template<>
void encode(ByteBuffer&, const k10engine::Game::View&);

template<>
void encode(ByteBuffer&, const k10engine::Game::View::Type&);
