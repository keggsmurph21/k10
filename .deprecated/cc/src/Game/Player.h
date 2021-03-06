#pragma once

#include <iostream>
#include <string.h>
#include <vector>

#include "Core/Action.h"
#include "Core/Building.h"
#include "Core/DevelopmentCard.h"
#include "Core/Edge.h"
#include "Core/Resource.h"
#include "Core/Vertex.h"
#include "Forward.h"
#include "Game/BoardView/Junction.h"
#include "Game/BoardView/Road.h"
#include "Game/ExecutionOptions.h"
#include "Game/Game.h"
#include "Game/Trade.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"

namespace k10engine::Game {

#define k10_DEFAULT_BANK_TRADE_RATE 4;
#define k10_MAX_RESOURCES_BEFORE_DISCARDING 7;

class Player {
public:
    PlayerId id() const { return m_id; }
    size_t index() const { return m_index; }

    std::vector<Action> get_available_actions() const;

    bool can_accept_trade() const { return m_can_accept_trade; }
    bool has_declined_trade() const { return m_has_declined_trade; }
    size_t num_to_discard() const { return m_num_to_discard; }

    const State::Vertex& vertex() const { return m_vertex; }

    bool has_heavy_purse() const;
    size_t num_built(const Building&) const;
    bool can_build(const Building&) const;
    bool can_trade() const;
    bool can_trade_with_bank() const;
    bool is_current_player() const;

    size_t num_resources() const;
    size_t num_played_development_cards() const;
    size_t num_unplayed_development_cards() const;
    size_t army_size() const { return m_army_size; }
    size_t longest_road() const { return m_longest_road; }

    size_t public_victory_points() const { return m_public_victory_points; }

    const std::vector<BoardView::Junction*>& cities() const { return m_cities; }
    const std::vector<BoardView::Road*>& roads() const { return m_roads; }
    const std::vector<BoardView::Junction*>& settlements() const { return m_settlements; }

    const Game* game() const { return m_game; }

    ~Player() {}

    bool operator!=(const Player&) const;
    bool operator==(const Player&) const;

    friend Game;
    friend std::ostream& operator<<(std::ostream&, const Player&);

    void encode(ByteBuffer&) const;
    static Player* decode(ByteBuffer&, const std::vector<BoardView::NodeView*>&);

private:
    Player(PlayerId id, size_t index, const Game* game)
        : m_id(id)
        , m_index(index)
        , m_game(game)
    {
    }

    PlayerId m_id; // value 0 means "not set"
    size_t m_index;
    const Game* m_game;

    // only used once we "start" a game
    void set_id(PlayerId id) { m_id = id; }

    // only used during deserialization
    void set_game(const Game* game) { m_game = game; }

    ResourceCounts m_resources;
    size_t count(const Resource&) const;

    ResourceCounts m_bank_trade_rates;
    size_t bank_trade_rate(const Resource&) const;

    std::vector<BoardView::Junction*> m_cities;
    std::vector<BoardView::Road*> m_roads;
    std::vector<BoardView::Junction*> m_settlements;

    std::vector<DevelopmentCard> m_played_development_cards;
    std::vector<DevelopmentCard> m_playable_development_cards;
    std::vector<DevelopmentCard> m_unplayable_development_cards;

    State::Vertex m_vertex{ State::Vertex::WaitForTurn };

    bool m_can_accept_trade{ false };
    bool m_has_declined_trade{ false };
    size_t m_num_to_discard{ 0 };
    size_t m_public_victory_points{ 0 };
    size_t m_private_victory_points{ 0 };
    size_t m_army_size{ 0 };
    size_t m_longest_road{ 0 };

    void set_vertex(State::Vertex vertex) { m_vertex = vertex; }

    void accrue_resources(const ResourceCounts&);
    void spend_resources(const ResourceCounts&);
    void collect_resource(Resource);

    void build_city(BoardView::Junction*, Options);
    void build_development_card(DevelopmentCard, Options);
    void build_road(BoardView::Road*, Options);
    void build_settlement(BoardView::Junction*, Options);

    void play_development_card(DevelopmentCard);

    void play_knight(const BoardView::Hex*);
    void play_monopoly(const Resource&);
    void play_road_building(BoardView::Road*, BoardView::Road*);
    void play_victory_point();
    void play_year_of_plenty(const Resource&, const Resource&);

    void set_can_accept_trade(bool can_accept_trade) { m_can_accept_trade = can_accept_trade; }
    void set_has_declined_trade(bool has_declined_trade) { m_has_declined_trade = has_declined_trade; }
    void set_num_to_discard(size_t num_to_discard) { m_num_to_discard = num_to_discard; }

    void set_longest_road(size_t longest_road) { m_longest_road = longest_road; }
    size_t private_victory_points() const { return m_private_victory_points; }

    void accept_trade_as_offerer();
    void accept_trade_as_acceptor();
    void cancel_trade();
    void decline_trade();
    void fail_trade(); // unable to find a trade partner
    void trade_with_bank();

    void move_robber();

    void steal_from(Player*);

    bool can_afford(const ResourceCounts&) const;
    bool can_afford(const Building&) const;

    std::vector<Action> get_actions_at_after_building_free_settlement() const;
    std::vector<Action> get_actions_at_after_discarding() const;
    std::vector<Action> get_actions_at_after_moving_robber() const;
    std::vector<Action> get_actions_at_after_rolling_seven() const;
    std::vector<Action> get_actions_at_choose_initial_resources() const;
    std::vector<Action> get_actions_at_root() const;
    std::vector<Action> get_actions_at_wait_for_turn() const;
    std::vector<Action> get_actions_at_wait_for_trade_responses() const;
};

} // namespace k10engine::Game

template<>
inline void encode(ByteBuffer& buf, k10engine::Game::Player* const& player)
{
    player->encode(buf);
}
