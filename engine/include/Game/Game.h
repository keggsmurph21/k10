#pragma once

#include <functional>
#include <iostream>
#include <optional>
#include <vector>

#include "Board/Graph.h"
#include "Core/Action.h"
#include "Core/Result.h"
#include "Core/Vertex.h"
#include "Game/BoardView/Hex.h"
#include "Game/BoardView/Junction.h"
#include "Game/BoardView/Road.h"
#include "Game/Dice.h"
#include "Game/ExecutionOptions.h"
#include "Game/Player.h"
#include "Game/Robber.h"
#include "Game/Trade.h"
#include "Scenario/Parameters.h"
#include "Scenario/Scenario.h"

namespace k10engine::Game {

// If a player has *greater than* these, then they get
// the corresponding bonus victory points.
#define k10_LARGEST_ARMY_THRESHOLD 2
#define k10_LONGEST_ROAD_THRESHOLD 4

#define MAX_NUM_TRADE_OFFERS_PER_TURN 42

class Player;

class Game {
public:
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
    bool has_current_trade() const { return m_current_trade != nullptr; }
    const Trade* current_trade() const { return m_current_trade; }

    const BoardView::Hex* robber_location() const { return m_robber.location(); }

    size_t get_dice_total() const;

    size_t turn() const { return m_turn; }
    size_t get_round() const; // turn() // #players

    const Player* has_largest_army() const { return m_has_largest_army; }
    size_t largest_army() const;
    const Player* has_longest_road() const { return m_has_longest_road; }
    size_t longest_road() const;

    size_t num_built(Building) const;

    const std::map<size_t, BoardView::Hex*>& hexes() const { return m_hexes; }
    const std::map<size_t, BoardView::Junction*>& junctions() const { return m_junctions; }
    const std::map<size_t, BoardView::Road*>& roads() const { return m_roads; }

    const Board::Graph* graph() const { return m_graph; }
    const Scenario::Scenario& scenario() const { return m_scenario; }

    const Player* current_player() const { return m_players.at(m_current_player_index); }
    const std::vector<Player*>& players() const { return m_players; }

    friend std::ostream& operator<<(std::ostream&, const Game&);

    Result execute_action(size_t player_id, const Action&);

    Game(const Board::Graph*,
         std::map<size_t, BoardView::Hex*>,
         std::map<size_t, BoardView::Junction*>,
         std::map<size_t, BoardView::Road*>,
         std::vector<DevelopmentCard>,
         const Scenario::Scenario&,
         const Scenario::Parameters&,
         int robber_index);
    ~Game();

private:
    Player* current_player() { return m_players.at(m_current_player_index); }

    const Board::Graph* m_graph;

    std::map<size_t, BoardView::Hex*> m_hexes;
    std::map<size_t, BoardView::Junction*> m_junctions;
    std::map<size_t, BoardView::Road*> m_roads;
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
    Trade* m_current_trade{ nullptr };

    size_t m_turn{ 0 };

    void move_robber(const Player*, const BoardView::Hex*);

    Player* m_has_largest_army{ nullptr };
    Player* m_has_longest_road{ nullptr };

    void recalculate_longest_road();
    void recalculate_largest_army(Player*);

    std::map<Building, size_t> m_buildings_built;
    void increment_num_built(Building);

    DevelopmentCard draw_development_card();

    void build_city(Player*, BoardView::Junction*, Options);
    DevelopmentCard build_development_card(Player*, Options);
    void build_settlement(Player*, BoardView::Junction*, Options);
    void build_road(Player*, BoardView::Road*, Options);

    void increment_turn();

    void set_can_steal(bool can_steal) { m_can_steal = can_steal; }
    void set_has_rolled(bool has_rolled) { m_has_rolled = has_rolled; }
    void set_is_trade_accepted(bool is_trade_accepted) { m_is_trade_accepted = is_trade_accepted; }
    void set_current_trade(Trade* trade);

    Result execute_accept_trade(Player*, const Action&);
    Result execute_build(Player*, const Action&);
    Result execute_cancel_trade(Player*, const Action&);
    Result execute_choose_initial_resources(Player*, const Action&);
    Result execute_collect_resources(Player*, const Action&);
    Result execute_decline_trade(Player*, const Action&);
    Result execute_discard(Player*, const Action&);
    Result execute_end_turn(Player*, const Action&);
    Result execute_fail_trade_unable_to_find_partner(Player*, const Action&);
    Result execute_move_robber(Player*, const Action&);
    Result execute_offer_trade(Player*, const Action&);
    Result execute_play_development_card(Player*, const Action&);
    Result execute_roll_dice(Player*, const Action&);
    Result execute_steal(Player*, const Action&);
    Result execute_to_root(Player*, const Action&);
    Result execute_trade_with_bank(Player*, const Action&);

    Result execute_build_city(Player*, const ActionArgument&);
    Result execute_build_development_card(Player*);
    Result execute_build_road(Player*, const ActionArgument&);
    Result execute_build_settlement(Player*, const ActionArgument&);

    Result execute_play_knight(Player*, const ActionArgument&);
    Result execute_play_monopoly(Player*, const ActionArgument&);
    Result execute_play_road_building(Player*, const ActionArgument&, const ActionArgument&);
    Result execute_play_victory_point(Player*);
    Result execute_play_year_of_plenty(Player*, const ActionArgument&, const ActionArgument&);
};

Game* initialize(const Board::Graph*, const Scenario::Scenario&, const Scenario::Parameters&);

} // namespace k10engine::Game
