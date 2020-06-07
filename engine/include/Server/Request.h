#pragma once

#include "Registrar.h"

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

    static const Request* decode(const char* buf, int len);

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

    static const RegisterUserRequest* decode(const char* buf, int len);
};

struct NewGameRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    FIXME m_parameters;
    NewGameRequest(Registrar::PlayerId player_id, Registrar::PlayerSecret player_secret, FIXME parameters)
        : Request(Type::JoinGame)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_parameters(parameters)
    {
    }

    static const NewGameRequest* decode(const char* buf, int len);
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

    static const JoinGameRequest* decode(const char* buf, int len);
};

struct LeaveGameRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    const GameId m_game_id;
    LeaveGameRequest(Registrar::PlayerId player_id, Registrar::PlayerSecret player_secret, GameId game_id)
        : Request(Type::JoinGame)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_game_id(game_id)
    {
    }

    static const LeaveGameRequest* decode(const char* buf, int len);
};

struct StartGameRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    const GameId m_game_id;
    StartGameRequest(Registrar::PlayerId player_id, Registrar::PlayerSecret player_secret, GameId game_id)
        : Request(Type::JoinGame)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_game_id(game_id)
    {
    }

    static const StartGameRequest* decode(const char* buf, int len);
};

struct MakeMoveRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    const GameId m_game_id;
    MakeMoveRequest(Registrar::PlayerId player_id, Registrar::PlayerSecret player_secret, GameId game_id)
        : Request(Type::JoinGame)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_game_id(game_id)
    {
    }

    static const MakeMoveRequest* decode(const char* buf, int len);
};

struct QueryRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    const GameId m_game_id;
    QueryRequest(Registrar::PlayerId player_id, Registrar::PlayerSecret player_secret, GameId game_id)
        : Request(Type::JoinGame)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_game_id(game_id)
    {
    }

    static const QueryRequest* decode(const char* buf, int len);
};

struct RegisterListenerRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    const GameId m_game_id;
    RegisterListenerRequest(Registrar::PlayerId player_id, Registrar::PlayerSecret player_secret, GameId game_id)
        : Request(Type::JoinGame)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_game_id(game_id)
    {
    }

    static const RegisterListenerRequest* decode(const char* buf, int len);
};

struct UnregisterListenerRequest final : public Request {
    const Registrar::PlayerId m_player_id;
    const Registrar::PlayerSecret m_player_secret;
    const GameId m_game_id;
    UnregisterListenerRequest(Registrar::PlayerId player_id, Registrar::PlayerSecret player_secret, GameId game_id)
        : Request(Type::JoinGame)
        , m_player_id(player_id)
        , m_player_secret(player_secret)
        , m_game_id(game_id)
    {
    }

    static const UnregisterListenerRequest* decode(const char* buf, int len);
};

} // namespace k10engine::Server
