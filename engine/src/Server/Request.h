#pragma once

#include "Core/Action.h"
#include "Scenario/Scenario.h"
#include "Server/Registrar.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"

#define MAX_BUF_LEN 1024
#define FIXME void*

namespace k10engine::Server {

using GameId = size_t;

struct Client;

struct Request {
    enum class Type {
        RegisterUser,
        NewGame,
        JoinGame,
        LeaveGame,
        StartGame,
        MakeMove,
        Query,
        RegisterListener,
        UnregisterListener,
    };

    const Type m_type;

    virtual ~Request() {}

protected:
    Request(/*Client& client,*/ Type type)
        /*: m_client(client)*/
        : m_type(type)
    {
    }

private:
    /*Client& m_client;*/
};

struct RegisterUserRequest final : public Request {
    const std::string m_name;
    const std::string m_secret;
    RegisterUserRequest(std::string name, std::string secret)
        : Request(Type::RegisterUser)
        , m_name(std::move(name))
        , m_secret(std::move(secret))
    {
    }
};

struct NewGameRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    // FIXME: Add ability to choose game board / scenario
    const Scenario::Parameters m_parameters;
    NewGameRequest(Registrar::PlayerId player_id,
                   Registrar::PlayerSecret player_secret,
                   Scenario::Parameters parameters)
        : Request(Type::NewGame)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_parameters(parameters)
    {
    }
};

struct JoinGameRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    const GameId m_game_id;
    JoinGameRequest(Registrar::PlayerId player_id, Registrar::PlayerSecret player_secret, GameId game_id)
        : Request(Type::JoinGame)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_game_id(game_id)
    {
    }
};

struct LeaveGameRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    const GameId m_game_id;
    LeaveGameRequest(Registrar::PlayerId player_id, Registrar::PlayerSecret player_secret, GameId game_id)
        : Request(Type::LeaveGame)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_game_id(game_id)
    {
    }
};

struct StartGameRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    const GameId m_game_id;
    StartGameRequest(Registrar::PlayerId player_id, Registrar::PlayerSecret player_secret, GameId game_id)
        : Request(Type::StartGame)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_game_id(game_id)
    {
    }
};

struct MakeMoveRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    const GameId m_game_id;
    const Game::Action m_action;
    MakeMoveRequest(Registrar::PlayerId player_id,
                    Registrar::PlayerSecret player_secret,
                    GameId game_id,
                    Game::Action action)
        : Request(Type::MakeMove)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_game_id(game_id)
        , m_action(action)
    {
    }
};

struct QueryRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    const GameId m_game_id;
    QueryRequest(Registrar::PlayerId player_id, Registrar::PlayerSecret player_secret, GameId game_id)
        : Request(Type::Query)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_game_id(game_id)
    {
    }
};

struct RegisterListenerRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    const GameId m_game_id;
    RegisterListenerRequest(Registrar::PlayerId player_id, Registrar::PlayerSecret player_secret, GameId game_id)
        : Request(Type::RegisterListener)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_game_id(game_id)
    {
    }
};

struct UnregisterListenerRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    const GameId m_game_id;
    UnregisterListenerRequest(Registrar::PlayerId player_id, Registrar::PlayerSecret player_secret, GameId game_id)
        : Request(Type::UnregisterListener)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_game_id(game_id)
    {
    }
};

} // namespace k10engine::Server

template<>
void encode(ByteBuffer&, const k10engine::Server::Request::Type&);

template<>
bool decode(ByteBuffer&, k10engine::Server::Request::Type&);

template<>
bool decode(ByteBuffer&, k10engine::Server::Request*&);

template<>
bool decode(ByteBuffer&, k10engine::Server::RegisterUserRequest*&);

template<>
bool decode(ByteBuffer&, k10engine::Server::NewGameRequest*&);

template<>
bool decode(ByteBuffer&, k10engine::Server::JoinGameRequest*&);

template<>
bool decode(ByteBuffer&, k10engine::Server::LeaveGameRequest*&);

template<>
bool decode(ByteBuffer&, k10engine::Server::StartGameRequest*&);

template<>
bool decode(ByteBuffer&, k10engine::Server::MakeMoveRequest*&);

template<>
bool decode(ByteBuffer&, k10engine::Server::QueryRequest*&);
