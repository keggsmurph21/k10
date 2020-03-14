#include <cassert>
#include <stdexcept>

#include "Game/Player.h"

namespace k10engine::Game {

std::vector<Action> Player::get_available_actions() const
{
    std::vector<Action> available_actions;
    // FIXME: Handle first two turns!!
    switch (m_vertex) {

    case State::Vertex::AfterBuildingFreeSettlement: {

        std::set<const BoardView::Road*> reachable_roads;

        // roads a distance of 1 away from our current roads
        for (const auto owned_road : roads()) {
            for (const auto junction_neighbor : owned_road->junction_neighbors()) {
                const auto junction = junction_neighbor.second;
                if (junction->owner() != nullptr && junction->owner() != this) {
                    continue; // can't build roads thru other settlements
                }
                for (const auto road_neighbor : junction->road_neighbors()) {
                    const auto reachable_road = road_neighbor.second;
                    if (reachable_road->owner() == nullptr) {
                        reachable_roads.insert(reachable_road);
                    }
                }
            }
        }

        // roads adjacent to our current settlements
        for (const auto& it : m_game->junctions()) {
            const auto junction = it.second;
            for (const auto road_neighbor : junction->road_neighbors()) {
                const auto road = road_neighbor.second;
                if (junction->owner() == this && road->owner() == nullptr) {
                    reachable_roads.insert(road);
                }
            }
        }

        for (const auto road : reachable_roads) {
            available_actions.push_back(
                { State::Edge::Build,
                  { { ActionArgumentType::BuildItemId, static_cast<size_t>(Building::Road) },
                    { ActionArgumentType::NodeId, road->index() } } });
        }

        return available_actions;
    }

    case State::Vertex::AfterDiscarding:
        if (is_current_player()) {
            if (num_to_discard() > 0) {
                return { { State::Edge::Discard,
                           { { ActionArgumentType::ResourceCount, num_to_discard() } } } };
            }
            if (m_game->should_wait_for_discard()) {
                return {}; // i.e., wait
            }
            for (const auto& it : m_game->hexes()) {
                const auto hex = it.second;
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
            return {}; // i.e., wait
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
        for (const auto& it : m_game->hexes()) {
            const auto hex = it.second;
            if (hex->index() != m_game->robber_location()->index()) {
                available_actions.push_back(
                    { State::Edge::MoveRobber, { { ActionArgumentType::NodeId, hex->index() } } });
            }
        }
        return available_actions;

    case State::Vertex::ChooseInitialResources:
        assert(settlements().size() == 2);
        return { { State::Edge::ChooseInitialResources,
                   { { ActionArgumentType::NodeId, settlements().at(0)->node()->index() } } },
                 { State::Edge::ChooseInitialResources,
                   { { ActionArgumentType::NodeId, settlements().at(1)->node()->index() } } } };

    case State::Vertex::GameOver:
        assert(false); // We shouldn't hit this

    case State::Vertex::Root:
        if (m_game->is_first_round()) {
            for (const auto& it : m_game->junctions()) {
                const auto junction = it.second;
                if (junction->is_settleable()) {
                    available_actions.push_back(
                        { State::Edge::Build,
                          { { ActionArgumentType::BuildItemId,
                              static_cast<size_t>(Building::Settlement) },
                            { ActionArgumentType::NodeId, junction->index() } } });
                }
            }
            return available_actions;
        }
        if (m_game->is_second_round()) {
            for (const auto& it : m_game->junctions()) {
                const auto junction = it.second;
                if (junction->is_settleable()) {
                    available_actions.push_back(
                        { State::Edge::Build,
                          { { ActionArgumentType::BuildItemId,
                              static_cast<size_t>(Building::Settlement) },
                            { ActionArgumentType::NodeId, junction->index() } } });
                }
            }
            return available_actions;
        }
        if (!m_game->has_rolled()) {
            return { { State::Edge::RollDice, {} } };
        }
        {
            const bool can_build_city = can_build(Building::City);
            const bool can_build_development_card = can_build(Building::DevelopmentCard);
            const bool can_build_road = can_build(Building::Road);
            const bool can_build_settlement = can_build(Building::Settlement);

            if (can_build_development_card) {
                available_actions.push_back(
                    { State::Edge::Build,
                      { { ActionArgumentType::BuildItemId,
                          static_cast<size_t>(Building::DevelopmentCard) } } });
            }

            std::set<const BoardView::Road*> reachable_roads;
            for (const auto owned_road : roads()) {
                for (const auto junction_neighbor : owned_road->junction_neighbors()) {
                    const auto junction = junction_neighbor.second;
                    if (junction->owner() != nullptr && junction->owner() != this) {
                        continue; // can't build roads thru other settlements
                    }
                    for (const auto road_neighbor : junction->road_neighbors()) {
                        const auto reachable_road = road_neighbor.second;
                        if (reachable_road->owner() == nullptr) {
                            reachable_roads.insert(reachable_road);
                        }
                    }
                }
            }

            for (const auto& it : m_game->junctions()) {
                const auto junction = it.second;
                if (can_build_city) {
                    if (junction->owner() == this) {
                        available_actions.push_back(
                            { State::Edge::Build,
                              { { ActionArgumentType::BuildItemId,
                                  static_cast<size_t>(Building::City) },
                                { ActionArgumentType::NodeId, junction->index() } } });
                    }
                }
                for (const auto road_neighbor : junction->road_neighbors()) {
                    const auto road = road_neighbor.second;
                    if (junction->owner() == this && road->owner() == nullptr) {
                        reachable_roads.insert(road);
                    }
                    if (can_build_settlement && junction->is_settleable()) {
                        if (road->owner() == this) {
                            available_actions.push_back(
                                { State::Edge::Build,
                                  { { ActionArgumentType::BuildItemId,
                                      static_cast<size_t>(Building::Settlement) },
                                    { ActionArgumentType::NodeId, junction->index() } } });
                        }
                    }
                }
            }

            if (can_build_road) {
                for (const auto road : reachable_roads) {
                    available_actions.push_back(
                        { State::Edge::Build,
                          { { ActionArgumentType::BuildItemId,
                              static_cast<size_t>(Building::Road) },
                            { ActionArgumentType::NodeId, road->index() } } });
                }
            }

            std::set<DevelopmentCard> seen_development_cards;
            for (const auto& development_card : m_playable_development_cards) {
                if (seen_development_cards.find(development_card) != seen_development_cards.end()) {
                    continue;
                }
                switch (development_card) {
                case DevelopmentCard::Knight:
                    for (const auto& it : m_game->hexes()) {
                        const auto hex = it.second;
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
                    // NB: This structured as ONE ROAD PER ACTION, even though we expect players to
                    //     "execute" it with (up to) TWO ROADS in the action.
                    for (const auto road : reachable_roads) {
                        available_actions.push_back(
                            { State::Edge::PlayDevelopmentCard,
                              { { ActionArgumentType::DevelopmentCardId,
                                  static_cast<size_t>(development_card) },
                                { ActionArgumentType::NodeId, road->index() } } });
                    }
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
                && num_resources() > 0) {
                // FIXME: Make sure we handle trading with the bank
                available_actions.push_back({ State::Edge::OfferTrade, {} });
            }
        }
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

bool Player::can_build(const Building& building) const
{
    const auto& num_built = m_game->num_built(building);
    const auto& num_buildable = m_game->scenario().count(building);
    const auto& cost = m_game->scenario().cost(building);
    return (num_built < num_buildable) && can_afford(*cost);
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

bool Player::is_current_player() const
{
    return m_game->current_player() == this;
}

bool Player::is_blocking() const // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::is_blocking");
}

bool Player::can_afford(const ResourceCounts& resource_counts) const
{
    return !(m_resources < resource_counts);
}

bool Player::can_afford(const Building& building) const
{
    const auto cost = game()->scenario().cost(building);
    assert(cost != nullptr);
    return can_afford(*cost);
}

std::ostream& operator<<(std::ostream& os, const Player& player)
{
    os << "Player{ " << player.index() << ", " << player.vertex() << " }";
    return os;
}

void Player::build_settlement(BoardView::Junction* junction_to_settle, Options options)
{
    if ((options & Options::NoCost) != Options::NoCost) {
        const auto& cost = m_game->scenario().cost(Building::Road);
        spend_resources(*cost);
    }

    junction_to_settle->set_owner(this);
    junction_to_settle->set_has_settlement();
    junction_to_settle->set_is_not_settleable();
    for (const auto& road_neighbor : junction_to_settle->road_neighbors()) {
        const auto road = road_neighbor.second;
        for (const auto& junction_neighbor : road->junction_neighbors()) {
            const auto junction = junction_neighbor.second;
            if (junction != junction_to_settle) {
                junction->set_is_not_settleable();
            }
        }
    }

    m_settlements.push_back(junction_to_settle);
    m_private_victory_points += 1;
    m_public_victory_points += 1;
}

void Player::build_road(BoardView::Road* road, Options options)
{
    if ((options & Options::NoCost) != Options::NoCost) {
        const auto& cost = m_game->scenario().cost(Building::Road);
        spend_resources(*cost);
    }

    road->set_owner(this);
    m_roads.push_back(road);
}

size_t Player::num_resources() const
{
    size_t total = 0;
    for (const auto& it : m_resources) {
        const auto& count = it.second;
        total += count;
    }
    return total;
}

void Player::accrue_resources(const ResourceCounts& counts)
{
    for (const auto& it : counts) {
        const auto& resource = it.first;
        const auto& count = it.second;
        if (m_resources.find(resource) == m_resources.end()) {
            m_resources[resource] = count;
        } else {
            m_resources[resource] += count;
        }
    }
}

void Player::spend_resources(const ResourceCounts& counts)
{
    assert(can_afford(counts));
    for (const auto& it : counts) {
        const auto& resource = it.first;
        const auto& count = it.second;
        m_resources.at(resource) -= count;
    }
}

} // namespace k10engine::Game
