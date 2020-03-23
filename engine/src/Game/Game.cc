#include <cassert>
#include <iostream>
#include <stdexcept>
#include <variant>

#include "Game/Game.h"

namespace k10engine::Game {

size_t Game::get_dice_total() const
{
    return m_dice.get_total();
}

bool Game::is_first_round() const
{
    return get_round() == 0;
}

bool Game::is_second_round() const
{
    return get_round() == 1;
}

bool Game::is_roll_seven() const
{
    return get_dice_total() == k10_ROBBER_DICE_ROLL;
}

bool Game::should_wait_for_discard() const // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Game::should_wait_for_discard");
}

bool Game::should_wait_for_trade() const
{
    for (const auto& player : m_players) {
        if (player->can_accept_trade() && !player->has_declined_trade()) {
            return true;
        }
    }
    return false;
}

void Game::set_current_trade(Trade* trade)
{
    delete m_current_trade;
    m_current_trade = trade;
    if (trade == nullptr) {
        for (const auto& player : m_players) {
            player->set_can_accept_trade(false);
            player->set_has_declined_trade(false);
        }
    } else {
        for (const auto& player : trade->offered_to) {
            if (player->can_afford(trade->to_offerer)) {
                player->set_can_accept_trade(true);
                player->set_has_declined_trade(false);
            }
        }
        ++m_num_trades_offered_this_turn;
    }
}

size_t Game::get_round() const
{
    return m_turn / m_players.size();
}

Game::Game(const Board::Graph* graph,
           std::map<size_t, BoardView::Hex*> hexes,
           std::map<size_t, BoardView::Junction*> junctions,
           std::map<size_t, BoardView::Road*> roads,
           std::vector<DevelopmentCard> deck,
           const Scenario::Scenario& scenario,
           const Scenario::Parameters& parameters,
           int robber_index)
    : m_graph(graph)
    , m_hexes(std::move(hexes))
    , m_junctions(std::move(junctions))
    , m_roads(std::move(roads))
    , m_deck(std::move(deck))
    , m_scenario(scenario)
    , m_robber(m_hexes.at(robber_index))
    , m_victory_points_goal(parameters.victory_points_goal)
{
    for (size_t i = 0; i < parameters.players_count; ++i) {
        m_players.push_back(new Player(i, this));
    }
    current_player()->set_vertex(State::Vertex::Root);
}

Game::~Game()
{
    for (auto hex : m_hexes) {
        delete hex.second;
    }
    for (auto junction : m_junctions) {
        delete junction.second;
    }
    for (auto road : m_roads) {
        delete road.second;
    }
    for (auto player : m_players) {
        delete player;
    }
    m_deck.clear();
    delete m_current_trade;
}

Game* initialize(const Board::Graph* graph,
                 const Scenario::Scenario& scenario,
                 const Scenario::Parameters& parameters)
{
    if (!scenario.is_valid(parameters)) {
        return nullptr;
    }

    const auto& deck =
        scenario.get_development_card_deck(parameters.development_card_iteration_type);
    const auto& ports = scenario.get_ports(parameters.port_iteration_type);
    const auto& resources = scenario.get_resources(parameters.resource_iteration_type);
    const auto& rolls = scenario.get_rolls(parameters.roll_iteration_type);

    std::map<size_t, BoardView::Hex*> hexes;
    std::map<size_t, BoardView::Junction*> junctions;
    std::map<size_t, BoardView::Road*> roads;

    std::map<const Board::Node*, BoardView::Hex*> hex_lookup;
    std::map<const Board::Node*, BoardView::Junction*> junction_lookup;
    std::map<const Board::Node*, BoardView::Road*> road_lookup;

    int robber_index = -1;

    size_t resource_index = 0;
    size_t roll_index = 0;

    for (const auto& node : graph->nodes()) {
        switch (node->type()) {
        case Board::NodeType::Hex: { // scope for const
            if (resource_index >= resources.size()) {
                return nullptr; // Too few resources
            }
            const auto& resource = resources.at(resource_index);
            BoardView::Hex* hex;
            if (std::holds_alternative<NonYieldingResource>(resource)) {
                if (robber_index != -1) {
                    return nullptr; // FIXME: Handle multiple robbers?
                }
                robber_index = static_cast<int>(node->index());
                hex = new BoardView::Hex(node, resource, 0);
            } else {
                if (roll_index >= rolls.size()) {
                    return nullptr; // Too few rolls
                }
                const auto roll = rolls.at(roll_index);
                ++roll_index;
                hex = new BoardView::Hex(node, resource, roll);
            }
            hex_lookup[node] = hex;
            hexes[node->index()] = hex;
            ++resource_index;
        } break;
        case Board::NodeType::Junction: { // scope for const
            const auto port = graph->port(*node);
            BoardView::Junction* junction;
            if (port == nullptr) {
                junction = new BoardView::Junction(node, {}, 0);
            } else {
                const auto port_index = port->index();
                if (port_index >= ports.size()) {
                    return nullptr; // Too few ports
                }
                const auto& port_spec = ports.at(port_index);
                junction =
                    new BoardView::Junction(node, port_spec.resources, port_spec.exchange_rate);
            }
            junction_lookup[node] = junction;
            junctions[node->index()] = junction;
        } break;
        case Board::NodeType::Road: { // scope for const
            auto road = new BoardView::Road(node);
            road_lookup[node] = road;
            roads[node->index()] = road;
        } break;
        case Board::NodeType::Ocean:
        case Board::NodeType::UnflippedHex:
            break; // do nothing
        }
    }

    for (auto hex_it : hexes) {
        const auto hex = hex_it.second;
        for (const auto& direction : Board::AllDirections) {
            const auto neighbor_node = graph->neighbor(hex->node(), direction);
            if (neighbor_node == nullptr) {
                continue;
            }
            if (junction_lookup.find(neighbor_node) != junction_lookup.end()) {
                const auto neighbor_junction = junction_lookup.at(neighbor_node);
                hex->add_neighbor(direction, neighbor_junction);
            }
        }
    }
    for (auto junction_it : junctions) {
        const auto junction = junction_it.second;
        for (const auto& direction : Board::AllDirections) {
            const auto neighbor_node = graph->neighbor(junction->node(), direction);
            if (neighbor_node == nullptr) {
                continue;
            }
            if (hex_lookup.find(neighbor_node) != hex_lookup.end()) {
                const auto neighbor_hex = hex_lookup.at(neighbor_node);
                junction->add_neighbor(direction, neighbor_hex);
            } else if (road_lookup.find(neighbor_node) != road_lookup.end()) {
                const auto neighbor_road = road_lookup.at(neighbor_node);
                junction->add_neighbor(direction, neighbor_road);
            }
        }
    }
    for (auto road_it : roads) {
        const auto road = road_it.second;
        for (const auto& direction : Board::AllDirections) {
            const auto neighbor_node = graph->neighbor(road->node(), direction);
            if (neighbor_node == nullptr) {
                continue;
            }
            if (junction_lookup.find(neighbor_node) != junction_lookup.end()) {
                const auto neighbor_junction = junction_lookup.at(neighbor_node);
                road->add_neighbor(direction, neighbor_junction);
            }
        }
    }

    if (robber_index == -1 || hexes.find(robber_index) == hexes.end()) {
        return nullptr; // something went wrong ... or no Desert given
    }

    return new Game(graph, hexes, junctions, roads, deck, scenario, parameters, robber_index);
}

static bool player_can_execute_edge(const Player* player, const Action& requested_action)
{
    const auto valid_actions = player->get_available_actions();
    for (const auto& valid_action : valid_actions) {
        if (valid_action.edge == requested_action.edge) {
            return true;
        }
    }
    return false;
}

static bool is_road_reachable_for(const BoardView::Road* road, const Player* player)
{
    if (road->owner() != nullptr) {
        return false;
    }
    for (const auto& junction_neighbor : road->junction_neighbors()) {
        const auto& junction = junction_neighbor.second;
        if (junction->owner() == player) {
            return true;
        }
        if (junction->owner() != nullptr) {
            continue;
        }
        for (const auto& road_neighbor : junction->road_neighbors()) {
            const auto& other_road = road_neighbor.second;
            if (other_road == road) {
                continue;
            }
            if (other_road->owner() == player) {
                return true;
            }
        }
    }
    return false;
}

static BoardView::Road* parse_road(const Game* game, const ActionArgument& arg)
{
    if (arg.type != ActionArgumentType::NodeId) {
        return nullptr;
    }
    const auto node = game->graph()->node(arg.value);
    if (node == nullptr) {
        return nullptr;
    }
    const auto road_it = game->roads().find(node->index());
    if (road_it == game->roads().end()) {
        return nullptr;
    }
    return road_it->second;
}

static BoardView::Junction* parse_junction(const Game* game, const ActionArgument& arg)
{
    if (arg.type != ActionArgumentType::NodeId) {
        return nullptr;
    }
    const auto node = game->graph()->node(arg.value);
    if (node == nullptr) {
        return nullptr;
    }
    const auto junction_it = game->junctions().find(node->index());
    if (junction_it == game->junctions().end()) {
        return nullptr;
    }
    return junction_it->second;
}

static Player* parse_player(const Game* game, const ActionArgument& arg)
{
    if (arg.type != ActionArgumentType::PlayerId) {
        return nullptr;
    }
    if (arg.value >= game->players().size()) {
        return nullptr;
    }
    return game->players().at(arg.value);
}

static std::optional<Resource> parse_resource(const Game* /* unused */, const ActionArgument& arg)
{
    if (arg.type != ActionArgumentType::GiveResourceType
        && arg.type != ActionArgumentType::TakeResourceType) {
        return {};
    }
    switch (static_cast<Resource>(arg.value)) { // yuck ...
    case Resource::Brick:
        return Resource::Brick;
    case Resource::Ore:
        return Resource::Ore;
    case Resource::Sheep:
        return Resource::Sheep;
    case Resource::Wheat:
        return Resource::Wheat;
    case Resource::Wood:
        return Resource::Wood;
    default:
        return {};
    }
}

static Trade*
parse_trade(const Game* game, Player* offerer, const std::vector<ActionArgument>& args)
{
    std::vector<Player*> offered_to;
    ResourceCounts give_resources{};
    ResourceCounts take_resources{};

    std::optional<Resource> resource;
    Player* parsed_player;

    for (const auto& arg : args) {
        switch (arg.type) {
        case ActionArgumentType::PlayerId:
            parsed_player = parse_player(game, arg);
            if (parsed_player == nullptr) {
                return nullptr;
            }
            if (parsed_player == offerer) {
                return nullptr;
            }
            offered_to.push_back(parsed_player);
            break;
        case ActionArgumentType::GiveResourceType:
            resource = parse_resource(game, arg);
            if (!resource) {
                return nullptr;
            }
            if (give_resources.find(*resource) == give_resources.end()) {
                give_resources[*resource] = 0;
            }
            give_resources[*resource] += 1;
            break;
        case ActionArgumentType::TakeResourceType:
            resource = parse_resource(game, arg);
            if (!resource) {
                return nullptr;
            }
            if (take_resources.find(*resource) == take_resources.end()) {
                take_resources[*resource] = 0;
            }
            take_resources[*resource] += 1;
            break;
        default:
            return nullptr;
        }
    }

    return new Trade{ offerer, offered_to, give_resources, take_resources };
}

static BoardView::Hex* parse_hex(const Game* game, const ActionArgument& arg)
{
    if (arg.type != ActionArgumentType::NodeId) {
        return nullptr;
    }
    const auto node = game->graph()->node(arg.value);
    if (node == nullptr) {
        return nullptr;
    }
    const auto hex_it = game->hexes().find(node->index());
    if (hex_it == game->hexes().end()) {
        return nullptr;
    }
    return hex_it->second;
}

Result Game::execute_accept_trade(Player* player, const Action& /* unused */)
{
    auto offerer = m_players.at(current_trade()->offerer->index());

    // std::cout << "before:" << std::endl;
    // std::cout << "  " << offerer->m_resources << std::endl;
    // std::cout << "  " << player->m_resources << std::endl;

    offerer->accrue_resources(current_trade()->to_offerer);
    offerer->spend_resources(current_trade()->from_offerer);

    player->accrue_resources(current_trade()->from_offerer);
    player->spend_resources(current_trade()->to_offerer);

    // std::cout << "after:" << std::endl;
    // std::cout << "  " << offerer->m_resources << std::endl;
    // std::cout << "  " << player->m_resources << std::endl;

    set_current_trade(nullptr);
    offerer->set_vertex(State::Vertex::Root);
    return { ResultType::Ok, {} };
}

Result Game::execute_build_city(Player*, const ActionArgument&)
{
    assert(false);
}

Result Game::execute_build_development_card(Player* player)
{
    if (!player->can_afford(Building::DevelopmentCard)) {
        return { ResultType::CannotAfford, {} };
    }

    const auto development_card = build_development_card(player, Options::None);

    if (is_game_over()) {
        assert(false);
    } else {
        player->set_vertex(State::Vertex::Root);
    }

    return { ResultType::Ok,
             { { ActionArgumentType::DevelopmentCardId, static_cast<size_t>(development_card) } } };
}

Result Game::execute_build_road(Player* player, const ActionArgument& arg)
{
    const auto road = parse_road(this, arg);
    if (road == nullptr) {
        return { ResultType::InvalidNodeId, {} };
    }

    if (!is_road_reachable_for(road, player)) {
        return { ResultType::InvalidNodeId, {} };
    }

    if (is_first_round() || is_second_round()) {
        build_road(player, road, Options::NoCost);
    } else if (player->can_afford(Building::Road)) {
        build_road(player, road, Options::None);
    } else {
        return { ResultType::CannotAfford, {} };
    }

    if (is_game_over()) {
        assert(false);
    } else if (is_first_round()) {
        player->set_vertex(State::Vertex::WaitForTurn);
        increment_turn();
    } else if (is_second_round()) {
        player->set_vertex(State::Vertex::ChooseInitialResources);
    } else {
        player->set_vertex(State::Vertex::Root);
    }

    return { ResultType::Ok, {} };
}

Result Game::execute_build_settlement(Player* player, const ActionArgument& arg)
{
    const auto junction = parse_junction(this, arg);
    if (junction == nullptr) {
        return { ResultType::InvalidNodeId, {} };
    }
    if (!junction->is_settleable()) {
        return { ResultType::JunctionNotSettleable, {} };
    }

    if (is_first_round() || is_second_round()) {
        build_settlement(player, junction, Options::NoCost);
    } else if (player->can_afford(Building::Settlement)) {
        build_settlement(player, junction, Options::None);
    } else {
        return { ResultType::CannotAfford, {} };
    }

    if (is_game_over()) {
        assert(false);
    } else if (is_first_round() || is_second_round()) {
        player->set_vertex(State::Vertex::AfterBuildingFreeSettlement);
    } else {
        player->set_vertex(State::Vertex::Root);
    }

    return { ResultType::Ok, {} };
}

Result Game::execute_build(Player* player, const Action& action)
{
    if (action.args.empty()) {
        return { ResultType::InvalidNumberOfArgs, {} };
    }
    if (action.args.at(0).type != ActionArgumentType::BuildItemId) {
        return { ResultType::InvalidArgumentType, {} };
    }
    const auto build_item = static_cast<Building>(action.args.at(0).value);
    switch (build_item) {
    case Building::City:
        if (action.args.size() != 2) {
            return { ResultType::InvalidNumberOfArgs, {} };
        }
        return execute_build_city(player, action.args.at(1));
    case Building::DevelopmentCard:
        if (action.args.size() != 1) {
            return { ResultType::InvalidNumberOfArgs, {} };
        }
        return execute_build_development_card(player);
    case Building::Road:
        if (action.args.size() != 2) {
            return { ResultType::InvalidNumberOfArgs, {} };
        }
        return execute_build_road(player, action.args.at(1));
    case Building::Settlement:
        if (action.args.size() != 2) {
            return { ResultType::InvalidNumberOfArgs, {} };
        }
        return execute_build_settlement(player, action.args.at(1));
    default:
        return { ResultType::BuildingIdOutOfRange, {} };
    }
}

Result Game::execute_cancel_trade(Player* player, const Action& /* unused */)
{
    set_current_trade(nullptr);
    player->set_vertex(State::Vertex::Root);
    return { ResultType::Ok, {} };
}

Result Game::execute_choose_initial_resources(Player* player, const Action& action)
{
    if (action.args.size() != 1) {
        return { ResultType::InvalidNumberOfArgs, {} };
    }
    if (action.args.at(0).type != ActionArgumentType::NodeId) {
        return { ResultType::InvalidArgumentType, {} };
    }
    const auto junction = parse_junction(this, action.args.at(0));
    if (junction == nullptr) {
        return { ResultType::InvalidNodeId, {} };
    }
    for (const auto& settlement : player->settlements()) {
        if (settlement == junction) {
            for (const auto& hex_it : settlement->hex_neighbors()) {
                const auto& hex_resource = hex_it.second->resource();
                if (std::holds_alternative<Resource>(hex_resource)) {
                    player->accrue_resources({ { std::get<Resource>(hex_resource), 1 } });
                }
            }
            player->set_vertex(State::Vertex::WaitForTurn);
            increment_turn();
            return { ResultType::Ok, {} };
        }
    }
    return { ResultType::InvalidNodeId, {} };
}

Result Game::execute_collect_resources(Player*, const Action&)
{
    assert(false);
}

Result Game::execute_decline_trade(Player* player, const Action& /* unused */)
{
    player->set_has_declined_trade(true);
    return { ResultType::Ok, {} };
}

Result Game::execute_discard(Player*, const Action&)
{
    assert(false);
}

Result Game::execute_end_turn(Player* player, const Action&)
{
    player->set_vertex(State::Vertex::WaitForTurn);
    increment_turn();
    return { ResultType::Ok, {} };
}

Result Game::execute_fail_trade_unable_to_find_partner(Player* player, const Action& /* unused */)
{
    set_current_trade(nullptr);
    player->set_vertex(State::Vertex::Root);
    return { ResultType::Ok, {} };
}

Result Game::execute_move_robber(Player*, const Action&)
{
    assert(false);
}

Result Game::execute_offer_trade(Player* player, const Action& action)
{
    const auto trade = parse_trade(this, player, action.args);
    if (trade == nullptr) {
        return { ResultType::InvalidTrade, {} };
    }
    if (trade->offered_to.empty()) {
        delete trade;
        return { ResultType::InvalidTrade, {} };
    }
    if (trade->from_offerer.empty()) {
        delete trade;
        return { ResultType::InvalidTrade, {} };
    }
    if (trade->to_offerer.empty()) {
        delete trade;
        return { ResultType::InvalidTrade, {} };
    }
    if (trade->from_offerer == trade->to_offerer) {
        delete trade;
        return { ResultType::InvalidTrade, {} };
    }
    if (!player->can_afford(trade->from_offerer)) {
        delete trade;
        return { ResultType::CannotAfford, {} };
    }

    set_current_trade(trade);
    player->set_vertex(State::Vertex::WaitForTradeResponses);

    return { ResultType::Ok, {} };
}

void Game::move_robber(const Player* player, const BoardView::Hex* hex)
{
    m_robber.set_location(hex);
    for (const auto& junction_it : hex->junction_neighbors()) {
        const auto& junction = junction_it.second;
        if (junction->owner() == nullptr) {
            continue;
        }
        if (junction->owner() == player) {
            continue;
        }
        set_can_steal(true);
    }
}

Result Game::execute_play_knight(Player* player, const ActionArgument& arg)
{
    const auto hex = parse_hex(this, arg);
    if (hex == nullptr) {
        return { ResultType::InvalidNodeId, {} };
    }

    if (hex->index() == robber_location()->index()) {
        return { ResultType::InvalidNodeId, {} };
    }

    move_robber(player, hex);
    player->play_knight(hex);
    player->set_vertex(State::Vertex::AfterMovingRobber);
    recalculate_largest_army(player);

    return { ResultType::Ok, {} };
}

Result Game::execute_play_monopoly(Player* player, const ActionArgument& arg)
{
    const auto resource = parse_resource(this, arg);
    if (!resource.has_value()) {
        return { ResultType::InvalidResourceType, {} };
    }

    for (auto& other_player : m_players) {
        if (other_player == player) {
            continue;
        }
        const auto count = other_player->count(*resource);
        other_player->spend_resources({ { *resource, count } });
        player->accrue_resources({ { *resource, count } });
    }

    player->play_monopoly(*resource);
    player->set_vertex(State::Vertex::Root);

    return { ResultType::Ok, {} };
}

Result Game::execute_play_road_building(Player* player,
                                        const ActionArgument& arg_0,
                                        const ActionArgument& arg_1)
{
    const auto road_0 = parse_road(this, arg_0);
    const auto road_1 = parse_road(this, arg_1);
    if (road_0 == nullptr || road_1 == nullptr) {
        return { ResultType::InvalidNodeId, {} };
    }

    if (!is_road_reachable_for(road_0, player)) {
        return { ResultType::InvalidNodeId, {} };
    }

    if (!is_road_reachable_for(road_1, player)) {
        // This road isn't reachable yet, but it might be reachable
        // once the first road is actually built.
        bool is_road_1_adjacent_to_road_0 = false;
        for (const auto& junction_it : road_0->junction_neighbors()) {
            if (is_road_1_adjacent_to_road_0) {
                break;
            }
            const auto& junction = junction_it.second;
            const auto& owner = junction->owner();
            if (owner != nullptr && owner != player) {
                continue;
            }
            for (const auto& road_it : junction->road_neighbors()) {
                const auto& road = road_it.second;
                if (road == road_1) {
                    is_road_1_adjacent_to_road_0 = true;
                    break;
                }
            }
        }
        if (!is_road_1_adjacent_to_road_0) {
            return { ResultType::InvalidNodeId, {} };
        }
    }

    build_road(player, road_0, Options::NoCost);
    build_road(player, road_1, Options::NoCost);

    player->play_road_building(road_0, road_0);
    player->set_vertex(State::Vertex::Root);

    return { ResultType::Ok, {} };
}

Result Game::execute_play_victory_point(Player* player)
{
    player->play_victory_point();
    player->set_vertex(State::Vertex::Root);

    return { ResultType::Ok, {} };
}

Result Game::execute_play_year_of_plenty(Player*, const ActionArgument&, const ActionArgument&)
{
    assert(false);
}

Result Game::execute_play_development_card(Player* player, const Action& action)
{
    if (action.args.empty()) {
        return { ResultType::InvalidNumberOfArgs, {} };
    }
    if (action.args.at(0).type != ActionArgumentType::DevelopmentCardId) {
        return { ResultType::InvalidArgumentType, {} };
    }
    const auto development_card = static_cast<DevelopmentCard>(action.args.at(0).value);
    switch (development_card) {
    case DevelopmentCard::Knight:
        if (action.args.size() != 2) {
            return { ResultType::InvalidNumberOfArgs, {} };
        }
        return execute_play_knight(player, action.args.at(1));
    case DevelopmentCard::Monopoly:
        if (action.args.size() != 2) {
            return { ResultType::InvalidNumberOfArgs, {} };
        }
        return execute_play_monopoly(player, action.args.at(1));
    case DevelopmentCard::RoadBuilding:
        if (action.args.size() != 3) {
            return { ResultType::InvalidNumberOfArgs, {} };
        }
        return execute_play_road_building(player, action.args.at(1), action.args.at(2));
    case DevelopmentCard::VictoryPoint:
        if (action.args.size() != 1) {
            return { ResultType::InvalidNumberOfArgs, {} };
        }
        return execute_play_victory_point(player);
    case DevelopmentCard::YearOfPlenty:
        if (action.args.size() != 3) {
            return { ResultType::InvalidNumberOfArgs, {} };
        }
        return execute_play_year_of_plenty(player, action.args.at(1), action.args.at(2));
    default:
        return { ResultType::DevelopmentCardIdOutOfRange, {} };
    }
}

Result Game::execute_roll_dice(Player*, const Action& action)
{
    if (action.args.empty()) {
        m_dice.roll();
#ifdef k10_ENABLE_ROLL_DICE_EXACT
    } else if (action.args.size() == 1) {
        if (action.args.at(0).type != ActionArgumentType::DiceRoll) {
            return { ResultType::InvalidArgumentType, {} };
        }
        const auto roll = action.args.at(0).value;
        if (roll < 2 || 12 < roll) {
            return { ResultType::DiceRollOutOfRange, {} };
        }
        m_dice.set_total(roll);
#endif
    } else {
        return { ResultType::InvalidNumberOfArgs, {} };
    }

    m_has_rolled = true;

    const auto dice_total = get_dice_total();
    for (const auto& hex_it : hexes()) {
        const auto& hex = hex_it.second;
        if (hex->roll_number() != dice_total) {
            continue;
        }
        const auto& hex_resource = hex->resource();
        if (std::holds_alternative<NonYieldingResource>(hex_resource)) {
            continue;
        }
        const auto& resource = std::get<Resource>(hex_resource);
        for (const auto& junction_neighbor : hex->junction_neighbors()) {
            const auto& junction = junction_neighbor.second;
            if (junction->owner() == nullptr) {
                continue;
            }
            const auto count = junction->has_city() ? 2 : 1;
            m_players.at(junction->owner()->index())->accrue_resources({ { resource, count } });
        }
    }

    return { ResultType::Ok, { { ActionArgumentType::DiceRoll, dice_total } } };
}

Result Game::execute_steal(Player* player, const Action& action)
{
    if (action.args.empty()) {
        return { ResultType::InvalidNumberOfArgs, {} };
    }
    const auto steal_from = parse_player(this, action.args.at(0));

    if (steal_from == player) {
        return { ResultType::InvalidPlayerId, {} };
    }

    if (steal_from->num_resources() == 0) {
        return { ResultType::InvalidPlayerId, {} };
    }

    bool is_adjacent = false;
    for (const auto junction_it : robber_location()->junction_neighbors()) {
        if (junction_it.second->owner() == steal_from) {
            is_adjacent = true;
            break;
        }
    }
    if (!is_adjacent) {
        return { ResultType::InvalidPlayerId, {} };
    }

    const auto stolen_resource = choose_from(steal_from->m_resources);
    steal_from->spend_resources({ { stolen_resource, 1 } });
    player->accrue_resources({ { stolen_resource, 1 } });

    set_can_steal(false);

    return { ResultType::Ok,
             { { ActionArgumentType::TakeResourceType, static_cast<size_t>(stolen_resource) } } };
}

Result Game::execute_to_root(Player* player, const Action&)
{
    player->set_vertex(State::Vertex::Root);
    return { ResultType::Ok, {} };
}

Result Game::execute_action(size_t player_id, const Action& action)
{
    if (player_id >= m_players.size()) {
        return { ResultType::InvalidPlayerId, {} };
    }
    const auto player = m_players.at(player_id);

    if (!player_can_execute_edge(player, action)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    switch (action.edge) {
    case State::Edge::AcceptTrade:
        return execute_accept_trade(player, action);
    case State::Edge::Build:
        return execute_build(player, action);
    case State::Edge::CancelTrade:
        return execute_cancel_trade(player, action);
    case State::Edge::ChooseInitialResources:
        return execute_choose_initial_resources(player, action);
    case State::Edge::CollectResources:
        return execute_collect_resources(player, action);
    case State::Edge::DeclineTrade:
        return execute_decline_trade(player, action);
    case State::Edge::Discard:
        return execute_discard(player, action);
    case State::Edge::EndTurn:
        return execute_end_turn(player, action);
    case State::Edge::FailTradeUnableToFindPartner:
        return execute_fail_trade_unable_to_find_partner(player, action);
    case State::Edge::MoveRobber:
        return execute_move_robber(player, action);
    case State::Edge::OfferTrade:
        return execute_offer_trade(player, action);
    case State::Edge::PlayDevelopmentCard:
        return execute_play_development_card(player, action);
    case State::Edge::RollDice:
        return execute_roll_dice(player, action);
    case State::Edge::Steal:
        return execute_steal(player, action);
    case State::Edge::ToRoot:
        return execute_to_root(player, action);
    }

    return { ResultType::InvalidEdgeChoice, {} };
}

void Game::increment_num_built(Building building)
{
    if (m_buildings_built.find(building) == m_buildings_built.end()) {
        m_buildings_built[building] = 0;
    }
    m_buildings_built[building] += 1;
}

DevelopmentCard Game::draw_development_card()
{
    const auto development_card = m_deck.at(m_deck_index);
    ++m_deck_index;
    return development_card;
}

DevelopmentCard Game::build_development_card(Player* player, Options options)
{
    const auto development_card = draw_development_card();
    player->build_development_card(development_card, options);
    increment_num_built(Building::DevelopmentCard);
    // FIXME: Check if we drew a VictoryPoint
    return development_card;
}

void Game::build_settlement(Player* player, BoardView::Junction* junction, Options options)
{
    player->build_settlement(junction, options);
    increment_num_built(Building::Settlement);
}

void Game::build_road(Player* player, BoardView::Road* road, Options options)
{
    player->build_road(road, options);
    recalculate_longest_road();
    increment_num_built(Building::Road);
}

bool Game::is_game_over() const
{
    for (const auto& player : m_players) {
        if (player->private_victory_points() == m_victory_points_goal) {
            return true;
        }
    }
    return false;
}

void Game::increment_turn()
{
    ++m_turn;
    if (m_turn == m_players.size()) { // NOLINT(bugprone-branch-clone)
        // The first round is now over.  Keep m_current_player_index the same.
    } else if (m_turn == m_players.size() * 2) { // NOLINT(bugprone-branch-clone)
        // The second round is now over.  Keep m_current_player_index the same.
    } else if (is_second_round()) {
        // It's the second round, so we iterate backwards.
        --m_current_player_index;
    } else {
        // The default case.
        ++m_current_player_index;
        if (m_current_player_index == m_players.size()) {
            m_current_player_index = 0;
        }
    }
    m_has_rolled = false;
    m_num_trades_offered_this_turn = 0;
    for (const auto& player : m_players) {
        for (const auto& development_card : player->m_unplayable_development_cards) {
            player->m_playable_development_cards.push_back(development_card);
        }
        player->m_unplayable_development_cards.clear();
    }
}

size_t Game::largest_army() const
{
    if (m_has_largest_army == nullptr) {
        return k10_LARGEST_ARMY_THRESHOLD;
    }
    return m_has_largest_army->army_size();
}

size_t Game::longest_road() const
{
    if (m_has_longest_road == nullptr) {
        return k10_LONGEST_ROAD_THRESHOLD;
    }
    return m_has_longest_road->longest_road();
}

size_t Game::num_built(Building building) const
{
    if (m_buildings_built.find(building) == m_buildings_built.end()) {
        return 0;
    }
    return m_buildings_built.at(building);
}

void Game::recalculate_longest_road()
{
    // FIXME: Implement me plz :^)
}

void Game::recalculate_largest_army(Player* player)
{
    if (m_has_largest_army == player) {
        return;
    }
    if (player->army_size() > largest_army()) {
        if (m_has_largest_army != nullptr) {
            m_has_largest_army->m_private_victory_points -= 2;
            m_has_largest_army->m_public_victory_points -= 2;
        }
        player->m_private_victory_points += 2;
        player->m_public_victory_points += 2;
        m_has_largest_army = player;
    }
}

std::ostream& operator<<(std::ostream& os, const Game& game)
{
    const auto robber_hex = game.robber_location();
    for (size_t y = 0; y < game.graph()->height(); ++y) {
        for (size_t x = 0; x < game.graph()->width(); ++x) {
            const auto node = game.graph()->node(x, y);
            if (node == nullptr) {
                os << " ";
                continue;
            }
            switch (node->type()) {
            case Board::NodeType::Ocean:
                os << '~';
                continue;
            case Board::NodeType::UnflippedHex:
                os << "?";
                continue;
            case Board::NodeType::Junction: {
                const auto junction = game.junctions().at(node->index());
                const auto owner = junction->owner();
                if (owner != nullptr) {
                    os << owner->index();
                    continue;
                }
                if (junction->is_port()) {
                    os << "*";
                    continue;
                }
                os << ".";
                continue;
            }
            case Board::NodeType::Road: {
                const auto road = game.roads().at(node->index());
                const auto owner = road->owner();
                if (owner != nullptr) {
                    os << owner->index();
                    continue;
                }
                if (game.graph()->has_neighbor(node, Board::Direction::Clock6)
                    || game.graph()->has_neighbor(node, Board::Direction::Clock12)) {
                    os << '|';
                    continue;
                }
                if (game.graph()->has_neighbor(node, Board::Direction::Clock2)
                    || game.graph()->has_neighbor(node, Board::Direction::Clock8)) {
                    os << '/';
                    continue;
                }
                if (game.graph()->has_neighbor(node, Board::Direction::Clock4)
                    || game.graph()->has_neighbor(node, Board::Direction::Clock10)) {
                    os << '\\';
                    continue;
                }
                assert(false);
            }
            case Board::NodeType::Hex: {
                const auto hex = game.hexes().at(node->index());
                if (hex == robber_hex) {
                    os << "X";
                    continue;
                }
                const auto abstract_resource = hex->resource();
                if (std::holds_alternative<NonYieldingResource>(abstract_resource)) {
                    switch (std::get<NonYieldingResource>(abstract_resource)) {
                    case NonYieldingResource::Desert:
                        os << "d";
                        continue;
                    }
                } else {
                    /*
                    const auto roll = hex->roll_number();
                    os << (roll >= 10 ? roll - 10 : roll);
                    continue;
                    */
                    switch (std::get<Resource>(abstract_resource)) {
                    case Resource::Brick:
                        os << "b";
                        continue;
                    case Resource::Ore:
                        os << "o";
                        continue;
                    case Resource::Sheep:
                        os << "s";
                        continue;
                    case Resource::Wheat:
                        os << "h";
                        continue;
                    case Resource::Wood:
                        os << "w";
                        continue;
                    }
                }
                assert(false);
            }
            }
            assert(false);
        }
        if (y < game.graph()->height() - 1) {
            os << std::endl;
        }
    }
    return os;
}

} // namespace k10engine::Game
