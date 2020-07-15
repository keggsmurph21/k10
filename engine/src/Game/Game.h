#pragma once

#include <functional>
#include <iostream>
#include <optional>
#include <vector>

#include "Board/Graph.h"
#include "Core/Action.h"
#include "Core/Result.h"
#include "Core/Vertex.h"
#include "Forward.h"
#include "Game/BoardView/Hex.h"
#include "Game/BoardView/Junction.h"
#include "Game/BoardView/Node.h"
#include "Game/BoardView/Road.h"
#include "Game/Dice.h"
#include "Game/ExecutionOptions.h"
#include "Game/Player.h"
#include "Game/Robber.h"
#include "Game/Trade.h"
#include "Scenario/Parameters.h"
#include "Scenario/Scenario.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"

namespace k10engine::Game {

// If a player has *greater than* these, then they get
// the corresponding bonus victory points.
#define k10_LARGEST_ARMY_THRESHOLD 2
#define k10_LONGEST_ROAD_THRESHOLD 4

#define MAX_NUM_TRADE_OFFERS_PER_TURN 42

class Game {
public:
    Game(const Board::Graph*,
         std::vector<BoardView::NodeView*>&,
         std::vector<DevelopmentCard>,
         const Scenario::Scenario&,
         const Scenario::Parameters&,
         BoardView::Hex* robber_location);
    Game(const Game&) = delete;
    ~Game();

    static Game* decode(ByteBuffer&);
    void encode(ByteBuffer&) const;

    bool can_steal() const { return m_can_steal; }
    bool has_rolled() const { return m_has_rolled; }
    bool is_game_over() const;
    bool is_trade_accepted() const { return m_is_trade_accepted; }
    bool is_first_round() const;
    bool is_second_round() const;
    bool is_roll_seven() const;
    bool should_wait_for_discard() const;
    bool should_wait_for_trade() const;

    size_t num_trades_offered_this_turn() const { return m_num_trades_offered_this_turn; };
    bool has_current_trade() const { return m_current_trade.has_value(); }
    const std::optional<Trade>& current_trade() const { return m_current_trade; }

    const BoardView::Hex* robber_location() const { return m_robber.location(); }

    size_t get_dice_total() const;

    size_t turn() const { return m_turn; }
    size_t get_round() const; // turn() // #players

    const Player* has_largest_army() const { return m_has_largest_army; }
    size_t largest_army() const;
    const Player* has_longest_road() const { return m_has_longest_road; }
    size_t longest_road() const;

    size_t num_built(Building) const;

    const Board::Graph* graph() const { return m_graph; }
    const Scenario::Scenario& scenario() const { return m_scenario; }

    Player& current_player() { return *m_players.at(m_current_player_index); }
    const Player& current_player() const { return *m_players.at(m_current_player_index); }

    const std::vector<Player*>& players() const { return m_players; }
    Player& player(size_t index) { return *m_players.at(index); }
    const Player& player(size_t index) const { return *m_players.at(index); }

    friend std::ostream& operator<<(std::ostream&, const Game&);

    Result execute_accept_trade(Player&);
    Result execute_build_city(Player&, BoardView::Junction*);
    Result execute_build_development_card(Player&);
    Result execute_build_road(Player&, BoardView::Road*);
    Result execute_build_settlement(Player&, BoardView::Junction*);
    Result execute_build(Player&, const Action&);
    Result execute_cancel_trade(Player&);
    Result execute_choose_initial_resources(Player&, const BoardView::Junction*);
    Result execute_decline_trade(Player&);
    Result execute_discard(Player&, const ResourceCounts&);
    Result execute_end_turn(Player&);
    Result execute_move_robber(Player&, const BoardView::Hex*);
    Result execute_offer_trade(Player&, const Trade&);
    Result execute_play_knight(Player&, const BoardView::Hex*);
    Result execute_play_monopoly(Player&, const Resource&);
    Result execute_play_road_building(Player&, BoardView::Road*, BoardView::Road*);
    Result execute_play_victory_point(Player&);
    Result execute_play_year_of_plenty(Player&, const Resource&, const Resource&);
    Result execute_roll_dice(Player&);
#ifdef k10_ENABLE_ROLL_DICE_EXACT
    Result execute_roll_dice(Player&, size_t roll);
#endif
    Result execute_steal(Player&, Player& steal_from);
    Result execute_to_root(Player&);
    Result execute_trade_with_bank(Player&, const Trade&);

    void for_each_hex(const std::function<void(const BoardView::Hex*)>&) const;
    void for_each_junction(const std::function<void(const BoardView::Junction*)>&) const;
    void for_each_road(const std::function<void(const BoardView::Road*)>&) const;

    BoardView::Hex* hex(size_t index);
    BoardView::Junction* junction(size_t index);
    BoardView::Road* road(size_t index);

    const BoardView::Hex* hex(size_t index) const;
    const BoardView::Junction* junction(size_t index) const;
    const BoardView::Road* road(size_t index) const;

    static std::optional<Game> initialize(const Board::Graph*, const Scenario::Scenario&, const Scenario::Parameters&);

private:
    const Board::Graph* m_graph;

    std::vector<BoardView::NodeView*> m_nodes;
    std::vector<DevelopmentCard> m_deck;

    const Scenario::Scenario& m_scenario;

    Dice m_dice;
    Robber m_robber;

    size_t m_victory_points_goal;

    std::vector<Player*> m_players;

    size_t m_deck_index{ 0 };
    size_t m_current_player_index{ 0 };

    bool m_can_steal{ false };
    bool m_has_rolled{ false };

    bool m_is_trade_accepted{ false };
    size_t m_num_trades_offered_this_turn{ 0 };
    std::optional<Trade> m_current_trade;

    size_t m_turn{ 0 };

    void move_robber(const Player&, const BoardView::Hex*);

    Player* m_has_largest_army{ nullptr };
    Player* m_has_longest_road{ nullptr };

    void recalculate_longest_road();
    void recalculate_largest_army(Player*);

    std::map<Building, size_t> m_buildings_built;
    void increment_num_built(Building);

    DevelopmentCard draw_development_card();

    void build_city(Player&, BoardView::Junction*, Options);
    DevelopmentCard build_development_card(Player&, Options);
    void build_settlement(Player&, BoardView::Junction*, Options);
    void build_road(Player&, BoardView::Road*, Options);

    void increment_turn();

    void set_can_steal(bool can_steal) { m_can_steal = can_steal; }
    void set_has_rolled(bool has_rolled) { m_has_rolled = has_rolled; }
    void set_is_trade_accepted(bool is_trade_accepted) { m_is_trade_accepted = is_trade_accepted; }
    void set_current_trade(std::optional<Trade> trade);

    // FIXME: Is there a nicer solution than this?
    Result _after_roll(Player& player);
};

} // namespace k10engine::Game

template<>
void encode(ByteBuffer&, const k10engine::Game::Game&);

template<>
bool decode(ByteBuffer&, k10engine::Game::Game*&);
