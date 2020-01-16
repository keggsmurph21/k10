#pragma once

#include <string.h>
#include <time.h>
#include <vector>

/**
 * See:
 *  - https://github.com/keggsmurph21/CatOnline/blob/module/src/app/models/game.js
 *  - https://github.com/keggsmurph21/CatOnline/blob/module/src/app/logic/logic.js
 *  -
 * https://docs.google.com/spreadsheets/d/1UlTewbihkhtMcIgH6p1RqhkqUcSufclM7UvAu6EgOcI/edit#gid=931117447
 */

namespace Game {

enum class GraphNode {
    ACCEPT_TRADE,
    ACCEPT_TRADE_OTHER,
    BUY_DEVELOPMENT_CARD,
    COLLECT,
    DISCARD,
    DISCARD_OTHER,
    END_GAME,
    END_TURN,
    FORTIFY,
    INIT_COLLECT,
    MOVE_ROBBER,
    OFFER_TRADE,
    PAVE,
    PLAY_KNIGHT,
    PLAY_MONOPOLY,
    PLAY_ROAD_BUILDING,
    PLAY_VICTORY_POINT,
    PLAY_YEAR_OF_PLENTY,
    ROLL,
    ROOT,
    SETTLE,
    STEAL,
    TRADE_WITH_BANK;
};

enum class GraphEdge {
    ACCEPT_TRADE,
    ACCEPT_TRADE_OTHER,
    AFTER_DISCARD_OTHER,
    AFTER_TRADE_OTHER,
    BUILD_CITY,
    BUILD_ROAD,
    BUILD_SETTLEMENT,
    BUY_DEVELOPMENT_CARD,
    CANCEL_TRADE,
    DECLINE_TRADE,
    DISCARD_MOVE_ROBBER,
    END_GAME,
    END_INIT,
    END_TURN,
    FAIL_TRADE,
    INIT2_BUILD_ROAD,
    INIT_BUILD_ROAD,
    INIT_COLLECT,
    INIT_SETTLE,
    NO_STEAL_ROBBER,
    OFFER_TRADE,
    PLAY_KNIGHT,
    PLAY_MONOPOLY,
    PLAY_ROAD_BUILDING,
    PLAY_VICTORY_POINT,
    PLAY_YEAR_OF_PLENTY,
    ROLL,
    ROLL_COLLECT,
    ROLL_DISCARD,
    ROLL_DISCARD_OTHER,
    ROLL_MOVE_ROBBER,
    STEAL_ROBBER,
    TAKE_TURN,
    TO_ROOT,
    TRADE_BANK;
};

class Settings {
public:
    Settings(std::string scenario, int victory_points_goal)
        : m_scenario(scenario)
        , m_victory_points_goal(victory_points_goal)
    {
    }

    ~Settings() {}

private:
    std::string m_scenario; // FIXME: should be an enum

    // FIXME: The following field is not necessary for the engine to know about
    // bool m_is_public;

    int m_victory_points_goal;

    // FIXME: The following two fields are redundant once we have an m_settings field
    // int m_num_humans;
    // int m_num_computers;

    // FIXME: The following two fields are redundant once we have
    // std::string m_port_style;  // FIXME: should be an enum
    // std::string m_tile_style;  // FIXME: should be an enum
};

class Game {
public:
    Game(std::vector<int> player_ids, Settings settings)
        : m_player_ids(player_ids)
        , m_settings(settings)
    {
    }

    ~Game()
    {
        m_player_ids.clear();
        ~m_settings();
    }

private:
    std::vector<int> m_player_ids;
    Settings m_settings;
};

} // namespace Game
