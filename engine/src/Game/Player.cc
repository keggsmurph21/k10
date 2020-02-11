#include <stdexcept>

#include "Game/Player.h"

namespace k10engine::Game {

Flags Player::get_flags() const
{
    Flags flags;
    flags.vertex = m_vertex;
    flags.can_accept_trade = can_accept_trade();
    flags.can_build = {
        can_build(Building::City),
        can_build(Building::DevelopmentCard),
        can_build(Building::Road),
        can_build(Building::Settlement),
    };
    flags.can_play{
        can_play(DevelopmentCard::Knight),       can_play(DevelopmentCard::Monopoly),
        can_play(DevelopmentCard::RoadBuilding), can_play(DevelopmentCard::VictoryPoint),
        can_play(DevelopmentCard::YearOfPlenty),
    };
    flags.can_steal = m_game->can_steal();
    flags.can_trade = can_trade();
    flags.can_trade_with_bank = can_trade_with_bank();
    flags.has_rolled = m_game->has_rolled();
    flags.is_current_player = is_current_player();
    flags.is_first_round = m_game->is_first_round();
    flags.is_game_over = m_game->is_game_over();
    flags.is_roll_seven = m_game->is_roll_seven();
    flags.is_second_round = m_game->is_second_round();
    flags.is_trade_accepted = m_game->is_trade_accepted();
    flags.should_wait_for_discard = m_game->should_wait_for_discard();
    flags.should_wait_for_trade = m_game->should_wait_for_trade();
    flags.num_to_discard = num_to_discard();
    return flags;
}

std::vector<Action> Player::get_available_actions(const Flags& flags) const
{
    (void)flags;
    std::vector<Action> actions;

    if (false) {
        actions.push_back({ State::Edge::AcceptTrade, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::AcceptTradeOther, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::AfterDiscardOther, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::AfterTradeOther, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::Build, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::CancelTrade, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::DeclineTrade, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::DiscardMoveRobber, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::EndGame, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::EndInit, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::EndTurn, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::FailTrade, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::InitBuildFirstRoad, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::InitBuildSecondRoad, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::InitCollect, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::InitSettle, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::NoStealRobber, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::OfferTrade, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::PlayDevelopmentCard, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::Roll, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::RollCollect, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::RollDiscard, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::RollDiscardOther, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::RollMoveRobber, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::StealRobber, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::TakeTurn, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::ToRoot, {} });
    }
    if (false) {
        actions.push_back({ State::Edge::TradeBank, {} });
    }
    return actions;
}

bool Player::has_heavy_purse() const // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::has_heavy_purse");
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool Player::can_build(const Building& building) const
{
    (void)building;
    throw std::invalid_argument("Not implemented: Player::can_build");
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool Player::can_play(const DevelopmentCard& development_card) const
{
    (void)development_card;
    throw std::invalid_argument("Not implemented: Player::can_play");
}

bool Player::can_trade() const // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::can_trade");
}

bool Player::can_trade_with_bank() const // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::can_trade_with_bank");
}

bool Player::is_current_player() const // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::is_current_player");
}

bool Player::is_blocking() const // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::is_blocking");
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool Player::can_afford(const ResourceCounts& resource_counts) const
{
    (void)resource_counts;
    throw std::invalid_argument("Not implemented: Player::can_afford");
}

} // namespace k10engine::Game
