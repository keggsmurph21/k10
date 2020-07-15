#include <cassert>
#include <stdexcept>

#include "Game/Player.h"

namespace k10engine::Game {

std::vector<Action> Player::get_actions_at_after_building_free_settlement() const
{
    std::set<const BoardView::Road*> reachable_roads;

    // roads a distance of 1 away from our current roads
    for (const auto owned_road : roads()) {
        owned_road->for_each_junction_neighbor([&](auto, const auto* junction) {
            if (junction->owner() != nullptr && junction->owner() != this) {
                return; // can't build roads thru other settlements
            }
            junction->for_each_road_neighbor([&](auto, const auto* road) {
                if (road->owner() == nullptr) {
                    reachable_roads.insert(road);
                }
            });
        });
    }

    // roads adjacent to our current settlements
    m_game->for_each_junction([&](const auto* junction) {
        junction->for_each_road_neighbor([&](auto, const auto* road) {
            if (junction->owner() == this && road->owner() == nullptr) {
                reachable_roads.insert(road);
            }
        });
    });

    std::vector<Action> available_actions;
    available_actions.reserve(reachable_roads.size());

    for (const auto road : reachable_roads) {
        available_actions.push_back({ State::Edge::BuildRoad,
                                      { { ActionArgumentType::BuildItemId, static_cast<size_t>(Building::Road) },
                                        { ActionArgumentType::NodeId, road->index() } } });
    }

    return available_actions;
}

std::vector<Action> Player::get_actions_at_after_discarding() const
{
    std::vector<Action> available_actions;
    if (is_current_player()) {
        if (num_to_discard() > 0) {
            return { { State::Edge::Discard, { { ActionArgumentType::ResourceCount, num_to_discard() } } } };
        }
        if (m_game->should_wait_for_discard()) {
            return {}; // i.e., wait
        }
        m_game->for_each_hex([&](const auto* hex) {
            if (hex->index() != m_game->robber_location()->index()) {
                available_actions.push_back(
                    { State::Edge::MoveRobber, { { ActionArgumentType::NodeId, hex->index() } } });
            }
        });
        return available_actions;
    }
    if (num_to_discard() > 0) {
        return { { State::Edge::Discard, { { ActionArgumentType::ResourceCount, num_to_discard() } } } };
    }
    return {}; // i.e., wait
}

std::vector<Action> Player::get_actions_at_after_moving_robber() const
{
    std::vector<Action> available_actions;
    if (m_game->can_steal()) {
        const auto robber_location = m_game->robber_location();
        std::set<const Player*> players_to_steal_from;
        robber_location->for_each_junction_neighbor([&](auto, const auto* junction) {
            const auto owner = junction->owner();
            if (owner != nullptr && owner != this && owner->num_resources() > 0) {
                players_to_steal_from.insert(owner);
            }
        });
        for (const auto player_to_steal_from : players_to_steal_from) {
            available_actions.push_back(
                { State::Edge::Steal, { { ActionArgumentType::PlayerId, player_to_steal_from->index() } } });
        }
        return available_actions;
    }
    return { { State::Edge::ToRoot, {} } };
}

std::vector<Action> Player::get_actions_at_after_rolling_seven() const
{
    if (num_to_discard() > 0) {
        return { { State::Edge::Discard, { { ActionArgumentType::ResourceCount, num_to_discard() } } } };
    }
    if (m_game->should_wait_for_discard()) {
        return {}; // i.e., wait
    }
    std::vector<Action> available_actions;
    m_game->for_each_hex([&](const auto* hex) {
        if (hex->index() != m_game->robber_location()->index()) {
            available_actions.push_back({ State::Edge::MoveRobber, { { ActionArgumentType::NodeId, hex->index() } } });
        }
    });
    return available_actions;
}

std::vector<Action> Player::get_actions_at_choose_initial_resources() const
{
    assert(settlements().size() == 2);
    return { { State::Edge::ChooseInitialResources,
               { { ActionArgumentType::NodeId, settlements().at(0)->node().index() } } },
             { State::Edge::ChooseInitialResources,
               { { ActionArgumentType::NodeId, settlements().at(1)->node().index() } } } };
}

std::vector<Action> Player::get_actions_at_root() const
{
    std::vector<Action> available_actions;
    if (m_game->is_first_round()) {
        m_game->for_each_junction([&](const auto* junction) {
            if (junction->is_settleable()) {
                available_actions.push_back(
                    { State::Edge::BuildSettlement,
                      { { ActionArgumentType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                        { ActionArgumentType::NodeId, junction->index() } } });
            }
        });
        return available_actions;
    }
    if (m_game->is_second_round()) {
        m_game->for_each_junction([&](const auto* junction) {
            if (junction->is_settleable()) {
                available_actions.push_back(
                    { State::Edge::BuildSettlement,
                      { { ActionArgumentType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                        { ActionArgumentType::NodeId, junction->index() } } });
            }
        });
        return available_actions;
    }
    if (!m_game->has_rolled()) {
        return { { State::Edge::RollDice, {} } };
    }
    {
        available_actions.push_back({ State::Edge::EndTurn, {} });

        const bool can_build_city = can_build(Building::City);
        const bool can_build_development_card = can_build(Building::DevelopmentCard);
        const bool can_build_road = can_build(Building::Road);
        const bool can_build_settlement = can_build(Building::Settlement);

        if (can_build_development_card) {
            available_actions.push_back(
                { State::Edge::BuildDevelopmentCard,
                  { { ActionArgumentType::BuildItemId, static_cast<size_t>(Building::DevelopmentCard) } } });
        }

        std::set<const BoardView::Road*> reachable_roads;
        for (const auto owned_road : roads()) {
            owned_road->for_each_junction_neighbor([&](auto, const auto* junction) {
                if (junction->owner() != nullptr && junction->owner() != this) {
                    return; // can't build roads thru other settlements
                }
                junction->for_each_road_neighbor([&](auto, const auto* road) {
                    if (road->owner() == nullptr) {
                        reachable_roads.insert(road);
                    }
                });
            });
        }

        m_game->for_each_junction([&](const auto* junction) {
            if (can_build_city) {
                if (junction->owner() == this) {
                    available_actions.push_back(
                        { State::Edge::BuildCity,
                          { { ActionArgumentType::BuildItemId, static_cast<size_t>(Building::City) },
                            { ActionArgumentType::NodeId, junction->index() } } });
                }
            }
            junction->for_each_road_neighbor([&](auto, const auto* road) {
                if (junction->owner() == this && road->owner() == nullptr) {
                    reachable_roads.insert(road);
                }
                if (can_build_settlement && junction->is_settleable()) {
                    if (road->owner() == this) {
                        available_actions.push_back(
                            { State::Edge::BuildSettlement,
                              { { ActionArgumentType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                                { ActionArgumentType::NodeId, junction->index() } } });
                    }
                }
            });
        });

        if (can_build_road) {
            for (const auto road : reachable_roads) {
                available_actions.push_back(
                    { State::Edge::BuildRoad,
                      { { ActionArgumentType::BuildItemId, static_cast<size_t>(Building::Road) },
                        { ActionArgumentType::NodeId, road->index() } } });
            }
        }

        std::set<DevelopmentCard> seen_development_cards;
        for (const auto& development_card : m_playable_development_cards) {
            if (seen_development_cards.find(development_card) != seen_development_cards.end()) {
                continue;
            }
            seen_development_cards.insert(development_card);
            switch (development_card) {
            case DevelopmentCard::Knight:
                m_game->for_each_hex([&](const auto* hex) {
                    if (hex->index() != m_game->robber_location()->index()) {
                        available_actions.push_back(
                            { State::Edge::PlayKnight,
                              { { ActionArgumentType::DevelopmentCardId, static_cast<size_t>(development_card) },
                                { ActionArgumentType::NodeId, hex->index() } } });
                    }
                });
                break;
            case DevelopmentCard::RoadBuilding:
                // NB: This structured as ONE ROAD PER ACTION, even though we expect players to
                //     "execute" it with (up to) TWO ROADS in the action.
                for (const auto road : reachable_roads) {
                    available_actions.push_back(
                        { State::Edge::PlayRoadBuilding,
                          { { ActionArgumentType::DevelopmentCardId, static_cast<size_t>(development_card) },
                            { ActionArgumentType::NodeId, road->index() } } });
                }
                break;
            case DevelopmentCard::Monopoly:
                available_actions.push_back(
                    { State::Edge::PlayMonopoly,
                      { { ActionArgumentType::DevelopmentCardId, static_cast<size_t>(development_card) } } });
                break;
            case DevelopmentCard::VictoryPoint:
                available_actions.push_back(
                    { State::Edge::PlayVictoryPoint,
                      { { ActionArgumentType::DevelopmentCardId, static_cast<size_t>(development_card) } } });
                break;
            case DevelopmentCard::YearOfPlenty:
                available_actions.push_back(
                    { State::Edge::PlayYearOfPlenty,
                      { { ActionArgumentType::DevelopmentCardId, static_cast<size_t>(development_card) } } });
                break;
            }
        }

        if (can_trade()) {
            available_actions.push_back({ State::Edge::OfferTrade, {} });
        }

        if (can_trade_with_bank()) {
            available_actions.push_back({ State::Edge::TradeWithBank, {} });
        }

        return available_actions;
    }
}

std::vector<Action> Player::get_actions_at_wait_for_turn() const
{
    if (is_current_player()) {
        return { { State::Edge::ToRoot, {} } };
    }
    if (num_to_discard() > 0) {
        return { { State::Edge::Discard, { { ActionArgumentType::ResourceCount, num_to_discard() } } } };
    }
    if (can_accept_trade() && !has_declined_trade()) {
        return { { State::Edge::AcceptTrade, {} }, { State::Edge::DeclineTrade, {} } };
    }
    return {};
}

std::vector<Action> Player::get_actions_at_wait_for_trade_responses() const
{
    if (m_game->is_trade_accepted()) {
        return { { State::Edge::AcceptTrade, {} } };
    }
    if (m_game->should_wait_for_trade()) {
        return { { State::Edge::CancelTrade, {} } }; // or do nothing and wait
    }
    assert(false); // We should never have no one waiting to decline
}

std::vector<Action> Player::get_available_actions() const
{
    std::vector<Action> available_actions;
    switch (m_vertex) {
    case State::Vertex::AfterBuildingFreeSettlement:
        return get_actions_at_after_building_free_settlement();
    case State::Vertex::AfterDiscarding:
        return get_actions_at_after_discarding();
    case State::Vertex::AfterMovingRobber:
        return get_actions_at_after_moving_robber();
    case State::Vertex::AfterRollingSeven:
        return get_actions_at_after_rolling_seven();
    case State::Vertex::ChooseInitialResources:
        return get_actions_at_choose_initial_resources();
    case State::Vertex::GameOver:
        assert(false); // We shouldn't hit this
    case State::Vertex::Root:
        return get_actions_at_root();
    case State::Vertex::WaitForTurn:
        return get_actions_at_wait_for_turn();
    case State::Vertex::WaitForTradeResponses:
        return get_actions_at_wait_for_trade_responses();
    }
    assert(false);
}

bool Player::has_heavy_purse() const
{
    return num_resources() > k10_MAX_RESOURCES_BEFORE_DISCARDING;
}

size_t Player::num_built(const Building& building) const
{
    size_t count = 0;
    switch (building) {
    case Building::City:
        for (const auto& settlement : m_settlements) {
            if (settlement->has_city()) {
                ++count;
            }
        }
        return count;
    case Building::DevelopmentCard:
        return m_played_development_cards.size() + m_playable_development_cards.size()
               + m_unplayable_development_cards.size();
    case Building::Road:
        return m_roads.size();
    case Building::Settlement:
        for (const auto& settlement : m_settlements) {
            if (!settlement->has_city()) {
                ++count;
            }
        }
        return count;
    }
    assert(false);
}

bool Player::can_build(const Building& building) const
{
    size_t num_built = m_game->num_built(building);
    size_t num_buildable = m_game->scenario().count(building);
    if (num_buildable == 0) {
        num_built = Player::num_built(building);
        num_buildable = m_game->scenario().count_per_player(building);
    }
    const auto& cost = m_game->scenario().cost(building);
    assert(cost != nullptr);
    return (num_built < num_buildable) && can_afford(*cost);
}

bool Player::can_trade() const
{
    return m_game->num_trades_offered_this_turn() < MAX_NUM_TRADE_OFFERS_PER_TURN && num_resources() > 0;
}

bool Player::can_trade_with_bank() const
{
    for (const auto& it : m_resources) {
        const auto& resource = it.first;
        const auto& count = it.second;
        if (count >= bank_trade_rate(resource)) {
            return true;
        }
    }
    return false;
}

bool Player::is_current_player() const
{
    return m_game->current_player() == *this;
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

void Player::build_city(BoardView::Junction* junction, Options options)
{
    if ((options & Options::NoCost) != Options::NoCost) {
        const auto& cost = m_game->scenario().cost(Building::City);
        assert(cost != nullptr);
        spend_resources(*cost);
    }

    assert(junction->owner() == this);
    junction->set_has_city();

    m_cities.push_back(junction);
    m_private_victory_points += 1;
    m_public_victory_points += 1;
}

void Player::build_settlement(BoardView::Junction* junction_to_settle, Options options)
{
    if ((options & Options::NoCost) != Options::NoCost) {
        const auto& cost = m_game->scenario().cost(Building::Settlement);
        assert(cost != nullptr);
        spend_resources(*cost);
    }

    junction_to_settle->set_owner(this);
    junction_to_settle->set_has_settlement();
    junction_to_settle->set_is_not_settleable();
    junction_to_settle->for_each_road_neighbor([&](auto, BoardView::Road* road) {
        road->for_each_junction_neighbor([&](auto, BoardView::Junction* junction) {
            if (junction != junction_to_settle) {
                junction->set_is_not_settleable();
            }
        });
    });

    if (junction_to_settle->is_port()) {
        const auto port_exchange_rate = junction_to_settle->port_exchange_rate();
        for (const auto resource : junction_to_settle->port_resources()) {
            if (port_exchange_rate < bank_trade_rate(resource)) {
                m_bank_trade_rates[resource] = port_exchange_rate;
            }
        }
    }

    m_settlements.push_back(junction_to_settle);
    m_private_victory_points += 1;
    m_public_victory_points += 1;
}

void Player::build_development_card(DevelopmentCard development_card, Options options)
{
    if ((options & Options::NoCost) != Options::NoCost) {
        const auto& cost = m_game->scenario().cost(Building::DevelopmentCard);
        assert(cost != nullptr);
        spend_resources(*cost);
    }

    m_unplayable_development_cards.push_back(development_card);
}

void Player::build_road(BoardView::Road* road, Options options)
{
    if ((options & Options::NoCost) != Options::NoCost) {
        const auto& cost = m_game->scenario().cost(Building::Road);
        assert(cost != nullptr);
        spend_resources(*cost);
    }

    road->set_owner(this);
    m_roads.push_back(road);
}

void Player::play_development_card(DevelopmentCard development_card)
{
    for (auto it = m_playable_development_cards.begin(); it != m_playable_development_cards.end(); ++it) {
        if (*it == development_card) {
            m_playable_development_cards.erase(it);
            m_played_development_cards.push_back(development_card);
            return;
        }
    }
    assert(false);
}

void Player::play_knight(const BoardView::Hex* /* unused */)
{
    ++m_army_size;
    play_development_card(DevelopmentCard::Knight);
}

void Player::play_monopoly(const Resource& /* unused */)
{
    play_development_card(DevelopmentCard::Monopoly);
}

void Player::play_road_building(BoardView::Road* /* unused */, BoardView::Road* /* unused */)
{
    play_development_card(DevelopmentCard::RoadBuilding);
}

void Player::play_victory_point()
{
    ++m_private_victory_points;
    ++m_public_victory_points;
    play_development_card(DevelopmentCard::VictoryPoint);
}

void Player::play_year_of_plenty(const Resource& /* unused */, const Resource& /* unused */)
{
    play_development_card(DevelopmentCard::YearOfPlenty);
}

size_t Player::num_played_development_cards() const
{
    return m_played_development_cards.size();
}

size_t Player::num_unplayed_development_cards() const
{
    return m_playable_development_cards.size() + m_unplayable_development_cards.size();
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
        if (count == 0) {
            continue;
        }
        m_resources.at(resource) -= count;
    }
}

size_t Player::count(const Resource& resource) const
{
    if (m_resources.find(resource) == m_resources.end()) {
        return 0;
    }
    return m_resources.at(resource);
}

size_t Player::bank_trade_rate(const Resource& resource) const
{
    if (m_bank_trade_rates.find(resource) == m_bank_trade_rates.end()) {
        return k10_DEFAULT_BANK_TRADE_RATE;
    }
    return m_bank_trade_rates.at(resource);
}

bool Player::operator!=(const Player& other) const
{
    return !(*this == other);
}

bool Player::operator==(const Player& other) const
{
    return game() == other.game() && index() == other.index();
}

void Player::encode(ByteBuffer& buf) const
{
    Encoder encoder(buf);
    encoder << m_index;
    encoder << m_resources;
    encoder << m_bank_trade_rates;

    encoder << m_cities.size();
    for (auto* city : m_cities) {
        encoder << city->index();
    }

    encoder << m_roads.size();
    for (auto* road : m_roads) {
        encoder << road->index();
    }

    encoder << m_settlements.size();
    for (auto* settlement : m_settlements) {
        encoder << settlement->index();
    }

    encoder << m_played_development_cards;
    encoder << m_playable_development_cards;
    encoder << m_unplayable_development_cards;
    encoder << m_vertex;
    encoder << m_can_accept_trade;
    encoder << m_has_declined_trade;
    encoder << m_num_to_discard;
    encoder << m_public_victory_points;
    encoder << m_private_victory_points;
    encoder << m_army_size;
    encoder << m_longest_road;
}

Player* Player::decode(ByteBuffer& buf, const std::vector<BoardView::NodeView*>& nodes)
{
    Decoder decoder(buf);
    size_t index;
    if (!decoder.decode(index))
        return nullptr;

    ResourceCounts resources;
    if (!decoder.decode(resources))
        return nullptr;

    ResourceCounts bank_trade_rates;
    if (!decoder.decode(bank_trade_rates))
        return nullptr;

    std::vector<BoardView::Junction*> cities;
    size_t n_cities;
    if (!decoder.decode(n_cities))
        return nullptr;
    cities.reserve(n_cities);
    for (size_t i = 0; i < n_cities; ++i) {
        size_t index;
        if (!decoder.decode(index))
            return nullptr;
        auto* node = nodes.at(index);
        if (node == nullptr)
            return nullptr;
        if (!node->is_junction())
            return nullptr;
        cities.push_back(static_cast<BoardView::Junction*>(node));
    }

    std::vector<BoardView::Road*> roads;
    size_t n_roads;
    if (!decoder.decode(n_roads))
        return nullptr;
    roads.reserve(n_roads);
    for (size_t i = 0; i < n_roads; ++i) {
        size_t index;
        if (!decoder.decode(index))
            return nullptr;
        auto* node = nodes.at(index);
        if (node == nullptr)
            return nullptr;
        if (!node->is_junction())
            return nullptr;
        roads.push_back(static_cast<BoardView::Road*>(node));
    }

    std::vector<BoardView::Junction*> settlements;
    size_t n_settlements;
    if (!decoder.decode(n_settlements))
        return nullptr;
    settlements.reserve(n_settlements);
    for (size_t i = 0; i < n_settlements; ++i) {
        size_t index;
        if (!decoder.decode(index))
            return nullptr;
        auto* node = nodes.at(index);
        if (node == nullptr)
            return nullptr;
        if (!node->is_junction())
            return nullptr;
        settlements.push_back(static_cast<BoardView::Junction*>(node));
    }

    std::vector<DevelopmentCard> played_development_cards;
    if (!decoder.decode(played_development_cards))
        return nullptr;

    std::vector<DevelopmentCard> playable_development_cards;
    if (!decoder.decode(playable_development_cards))
        return nullptr;

    std::vector<DevelopmentCard> unplayable_development_cards;
    if (!decoder.decode(unplayable_development_cards))
        return nullptr;

    State::Vertex vertex;
    if (!decoder.decode(vertex))
        return nullptr;

    bool can_accept_trade;
    if (!decoder.decode(can_accept_trade))
        return nullptr;

    bool has_declined_trade;
    if (!decoder.decode(has_declined_trade))
        return nullptr;

    size_t num_to_discard;
    if (!decoder.decode(num_to_discard))
        return nullptr;

    size_t public_victory_points;
    if (!decoder.decode(public_victory_points))
        return nullptr;

    size_t private_victory_points;
    if (!decoder.decode(private_victory_points))
        return nullptr;

    size_t army_size;
    if (!decoder.decode(army_size))
        return nullptr;

    size_t longest_road;
    if (!decoder.decode(longest_road))
        return nullptr;

    auto* player = new Player(index, nullptr);
    player->m_resources = std::move(resources);
    player->m_bank_trade_rates = std::move(bank_trade_rates);
    player->m_cities = std::move(cities);
    player->m_roads = std::move(roads);
    player->m_settlements = std::move(settlements);
    player->m_played_development_cards = std::move(played_development_cards);
    player->m_playable_development_cards = std::move(playable_development_cards);
    player->m_unplayable_development_cards = std::move(unplayable_development_cards);
    player->m_vertex = vertex;
    player->m_can_accept_trade = can_accept_trade;
    player->m_has_declined_trade = has_declined_trade;
    player->m_num_to_discard = num_to_discard;
    player->m_public_victory_points = public_victory_points;
    player->m_private_victory_points = private_victory_points;
    player->m_army_size = army_size;
    player->m_longest_road = longest_road;
    return player;
}

} // namespace k10engine::Game
