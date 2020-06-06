#pragma once

#include <optional>

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

    static const Request* parse(const char* buf, int len);

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

    static const RegisterUserRequest* parse(const char* buf, int len);
};

struct NewGameRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    FIXME parameters;

    static const NewGameRequest* parse(const char* buf, int len);
};

struct JoinGameRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;

    static const JoinGameRequest* parse(const char* buf, int len);
};

struct LeaveGameRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;

    static const LeaveGameRequest* parse(const char* buf, int len);
};

struct StartGameRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;

    static const StartGameRequest* parse(const char* buf, int len);
};

struct MakeMoveRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;

    static const MakeMoveRequest* parse(const char* buf, int len);
};

struct QueryRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;

    static const QueryRequest* parse(const char* buf, int len);
};

struct RegisterListenerRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;

    static const RegisterListenerRequest* parse(const char* buf, int len);
};

struct UnregisterListenerRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;

    static const UnregisterListenerRequest* parse(const char* buf, int len);
};

} // namespace k10engine::Server
