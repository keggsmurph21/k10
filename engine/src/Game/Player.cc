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
                if (hex->index() != m_game->robber_location()->index()) {
                    available_actions.push_back(
                        { State::Edge::MoveRobber,
                          { { ActionArgumentType::NodeId, hex->index() } } });
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
            const auto robber_location = m_game->robber_location();
            std::set<const Player*> players_to_steal_from;
            for (const auto& junction_neighbor : robber_location->junction_neighbors()) {
                const auto junction = junction_neighbor.second;
                const auto owner = junction->owner();
                if (owner != nullptr && owner != this) {
                    players_to_steal_from.insert(owner);
                }
            }
            for (const auto player_to_steal_from : players_to_steal_from) {
                available_actions.push_back(
                    { State::Edge::Steal,
                      { { ActionArgumentType::PlayerId, player_to_steal_from->index() } } });
            }
            return available_actions;
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
        for (const auto& hex : m_game->hexes()) {
            if (hex->index() != m_game->robber_location()->index()) {
                available_actions.push_back(
                    { State::Edge::MoveRobber, { { ActionArgumentType::NodeId, hex->index() } } });
            }
        }
        return available_actions;

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
        {
            const bool can_build_development_card = can_build(Building::DevelopmentCard);

            if (can_build_development_card) {
                available_actions.push_back(
                    { State::Edge::Build,
                      { { ActionArgumentType::BuildItemId,
                          static_cast<size_t>(Building::DevelopmentCard) } } });
            }

            std::set<DevelopmentCard> seen_development_cards;
            for (const auto& development_card : m_playable_development_cards) {
                if (seen_development_cards.find(development_card) != seen_development_cards.end()) {
                    continue;
                }
                switch (development_card) {
                case DevelopmentCard::Knight:
                    for (const auto& hex : m_game->hexes()) {
                        if (hex->index() != m_game->robber_location()->index()) {
                            available_actions.push_back(
                                { State::Edge::PlayDevelopmentCard,
                                  { { ActionArgumentType::DevelopmentCardId,
                                      static_cast<size_t>(development_card) },
                                    { ActionArgumentType::NodeId, hex->index() } } });
                        }
                    }
                    break;
                case DevelopmentCard::RoadBuilding:
                    // FIXME: generate list: nodes to use road-building with
                    break;
                case DevelopmentCard::Monopoly:
                case DevelopmentCard::VictoryPoint:
                case DevelopmentCard::YearOfPlenty:
                    available_actions.push_back({ State::Edge::PlayDevelopmentCard,
                                                  { { ActionArgumentType::DevelopmentCardId,
                                                      static_cast<size_t>(development_card) } } });
                    break;
                }
            }

            if (m_num_trades_offered_this_turn < MAX_NUM_TRADE_OFFERS_PER_TURN
                && m_resources.size() > 0) {
                // FIXME: Make sure we handle trading with the bank
                available_actions.push_back({ State::Edge::OfferTrade, {} });
            }
        }
        // FIXME: generate list: nodes to build road
        // FIXME: generate list: nodes to build settlement
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
    }
    assert(false);
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

std::ostream& operator<<(std::ostream& os, const Player& player)
{
    os << "Player{ " << player.index() << ", " << player.vertex() << " }";
    return os;
}

} // namespace k10engine::Game
