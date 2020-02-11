#include <cassert>
#include <stdexcept>

#include "Game/Player.h"

namespace k10engine::Game {

std::vector<Action> Player::get_available_actions() const
{
    std::vector<Action> available_actions;
    // FIXME: Handle first two turns!!
    switch (m_vertex) {

    case State::Vertex::AfterDiscarding:
        if (is_current_player()) {
            if (num_to_discard() > 0) {
                return { { State::Edge::Discard,
                           { { ActionArgumentType::ResourceCount, num_to_discard() } } } };
            }
            if (m_game->should_wait_for_discard()) {
                return {}; // i.e., wait
            }
            for (const auto& hex : m_game->hexes()) {
                if (hex.index() != m_game->robber_location()->index()) {
                    available_actions.push_back(
                        { State::Edge::MoveRobber,
                          { { ActionArgumentType::NodeId, hex.index() } } });
                }
            }
            return available_actions;
        } else {
            if (num_to_discard() > 0) {
                return { { State::Edge::Discard,
                           { { ActionArgumentType::ResourceCount, num_to_discard() } } } };
            }
            return { { State::Edge::WaitForTurn, {} } };
        }

    case State::Vertex::AfterMovingRobber:
        if (m_game->can_steal()) {
            const auto& robber_location = m_game->robber_location();
            (void)robber_location;
            // for (const auto& junction : m_game->m_graph
            // FIXME: generate a list of players to steal from
            return { { State::Edge::Steal, {} } };
        }
        return { { State::Edge::ToRoot, {} } };

    case State::Vertex::AfterRoll:
        if (!m_game->is_roll_seven()) {
            return { { State::Edge::CollectResources, {} } };
        }
        if (num_to_discard() > 0) {
            return { { State::Edge::Discard,
                       { { ActionArgumentType::ResourceCount, num_to_discard() } } } };
        }
        if (m_game->should_wait_for_discard()) {
            return {}; // i.e., wait
        }
        // FIXME: generate a list of possible cards to discard
        return { { State::Edge::MoveRobber, {} } };

    case State::Vertex::GameOver:
        assert(false); // We shouldn't hit this

    case State::Vertex::Root:
        if (m_game->is_first_round()) {
            // FIXME: do something!
            return {};
        }
        if (m_game->is_second_round()) {
            // FIXME: do something!
            return {};
        }
        if (!m_game->has_rolled()) {
            return { { State::Edge::RollDice, {} } };
        }
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

    case State::Vertex::WaitForTurn:
        if (is_current_player()) {
            return { { State::Edge::ToRoot, {} } };
        }
        if (num_to_discard() > 0) {
            return { { State::Edge::Discard,
                       { { ActionArgumentType::ResourceCount, num_to_discard() } } } };
        }
        if (can_accept_trade()) {
            return { { State::Edge::AcceptTrade, {} }, { State::Edge::DeclineTrade, {} } };
        }
        return {};

    case State::Vertex::WaitingForTradeResponses:
        if (m_game->is_trade_accepted()) {
            return { { State::Edge::AcceptTrade, {} } };
        }
        if (m_game->should_wait_for_trade()) {
            return { { State::Edge::CancelTrade, {} } }; // or do nothing and wait
        }
        return { { State::Edge::FailTradeUnableToFindPartner, {} } };

    default:
        assert(false); // NOT REACHED
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
