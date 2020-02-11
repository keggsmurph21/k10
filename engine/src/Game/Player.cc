#include <cassert>
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
    // FIXME: Handle first two turns!!
    switch (flags.vertex) {

    case State::Vertex::AfterBuilding:
        if (flags.is_game_over) {
            return { { State::Edge::EndGame, {} } };
        } else {
            { { State::Edge::ToRoot, {} } };
        }

    case State::Vertex::AfterDiscarding:
        if (flags.is_current_player) {
            if (flags.num_to_discard > 0) {
                // FIXME: generate a list of possible cards to discard
                return { { State::Edge::Discard, {} } };
            } else if (flags.should_wait_for_discard) {
                return {}; // i.e., wait
            } else {
                // FIXME: generate a list of available robber locations
                return { { State::Edge::MoveRobber, {} } };
            }
        } else {
            if (flags.num_to_discard > 0) {
                // FIXME: generate a list of possible cards to discard
                return { { State::Edge::Discard, {} } };
            } else {
                return { { State::Edge::WaitForTurn, {} } };
            }
        }

    case State::Vertex::AfterMovingRobber:
        if (flags.can_steal) {
            // FIXME: generate a list of players to steal from
            return { { State::Edge::Steal, {} } };
        }
        return { { State::Edge::ToRoot } };

    case State::Vertex::AfterRoll:
        if (!flags.is_roll_seven) {
            return { { State::Edge::CollectResources, {} } };
        } else if (flags.num_to_discard > 0) {
            // FIXME: generate a list of possible cards to discard
            return { { State::Edge::Discard, {} } };
        } else if (flags.should_wait_for_discard) {
            return {}; // i.e., wait
        } else {
            // FIXME: generate a list of possible cards to discard
            return { { State::MoveRobber, {} } };
        }

    case State::Vertex::GameOver:
        std::assert(false); // We shouldn't hit this

    case State::Vertex::Root:
        if (flags.is_first_turn) {
            // FIXME: do something!
            return {};
        } else if (flags.is_second_turn) {
            // FIXME: do something!
            return {};
        } else if (!flags.has_rolled) {
            return { { State::Edge::RollDice, {} } };
        } else {
            std::vector<Action> available_actions;
            // FIXME: generate list: settlements to build city
            // FIXME: check possibility of buying development card
            // FIXME: generate list: nodes to build road
            // FIXME: generate list: nodes to build settlement
            // FIXME: generate list: hexes to move robber to with a knight
            // FIXME: generate list: resource to choose with a monopoly
            // FIXME: generate list: nodes to use road-building with
            // FIXME: check possibility of playing victory-point
            // FIXME: generate list: resources to choose with year-of-plenty
            // FIXME: generate list: trades to offer people
            // FIXME: generate list: trades to offer bank
            return available_actions;
        }

    case State::Vertex::WaitForTurn:
        if (flags.is_current_player) {
            return { { State::Edge::ToRoot, {} } };
        } else if (flags.num_to_discard > 0) {
            // FIXME: generate a list of possible cards to discard
            return { { State::Edge::Discard, {} } };
        } else if (flags.can_accept_trade) {
            return { { State::Edge::AcceptTrade, {} }, { State::Edge::DeclineTrade, {} } };
        } else {
            return {};
        }

    case State::Vertex::WaitingForTradeResponses:
        if (flags.trade_is_accepted) {
            return { { State::Edge::AcceptTrade, {} } };
        } else if (flags.should_wait_for_trade) {
            return { { State::Edge::CancelTrade, {} } }; // or do nothing and wait
        } else {
            return { { State::Edge::FailTradeUnableToFindPartner, {} } };
        }
    }
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
