#pragma once

#include "Registrar.h"

#define FIXME (void*)

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
        RegisterClient,
        UnregisterClient,
    };

protected:
    Request(Client& client, Type type)
        : client(client)
        , type(type)
    {
    }

private:
    Client& client;
    Type type;
};

struct RegisterUserRequest final : public Request {
    const char name[255];
    const u8 name_len;
    const char secret[255];
    const u8 secret_len;
};

struct NewGameRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    FIXME parameters;
};

struct JoinGameRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;
};

struct LeaveGameRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;
};

struct StartGameRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;
};

struct MakeMoveRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;
};

struct QueryRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;
};

struct RegisterClientRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;
};

struct UnregisterClientRequest final : public Request {
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
    GameId game_id;
};

struct Response {
    enum class Type {
        RegisterUser,
        NewGame,
        JoinGame,
        LeaveGame,
        StartGame,
        MakeMove,
        Query,
        RegisterClient,
        UnregisterClient,
        GameChanged,
    };

protected:
    Response(Type type)
        : type(type)
    {
    }

private:
    Type type;
};

struct RegisterUserResponse final : public Response {
    bool success;
    Registrar::PlayerId player_id;
    Registrar::PlayerSecret player_secret;
};

} // namespace k10engine::Server
