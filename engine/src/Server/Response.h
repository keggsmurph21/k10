#pragma once

#include "Server/Registrar.h"
#include "Util/ByteBuffer.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"
#include "Util/Types.h"

namespace k10engine::Server {

struct Response {
    enum class Type {
        Error,
        RegisterUser,
        NewGame,
        JoinGame,
        LeaveGame,
        StartGame,
        MakeMove,
        Query,
        RegisterListener,
        UnregisterListener,
        GameChanged,
    };

    enum class ErrorCode {
        Malformed,     // couldn't decode it
        NotRegistered, // registration failed
        NotFound,      // invalid game id
        Rejected,      // engine refused it
    };

    u16 m_client_id;
    u16 m_request_id;
    const Type m_type;

    virtual ~Response() {}

protected:
    Response(Type type)
        : m_type(type)
    {
    }
};

struct ErrorResponse final : public Response {
    const Response::ErrorCode m_code;
    ErrorResponse(Response::ErrorCode code)
        : Response(Response::Type::Error)
        , m_code(code)
    {
    }
};

struct RegisterUserResponse final : public Response {
    const std::optional<Registrar::Registration> m_registration;
    RegisterUserResponse(std::optional<Registrar::Registration> registration)
        : Response(Response::Type::RegisterUser)
        , m_registration(registration)
    {
    }
};

struct NewGameResponse final : public Response {
    Game::GameId m_game_id;
    NewGameResponse(Game::GameId game_id)
        : Response(Response::Type::NewGame)
        , m_game_id(game_id)
    {
    }
};

struct JoinGameResponse final : public Response {
    JoinGameResponse()
        : Response(Response::Type::JoinGame)
    {
    }
};

struct LeaveGameResponse final : public Response {
    LeaveGameResponse()
        : Response(Response::Type::LeaveGame)
    {
    }
};

struct StartGameResponse final : public Response {
    StartGameResponse()
        : Response(Response::Type::StartGame)
    {
    }
};

struct MakeMoveResponse final : public Response {
    MakeMoveResponse()
        : Response(Response::Type::MakeMove)
    {
    }
};

struct QueryResponse final : public Response {
    QueryResponse()
        : Response(Response::Type::Query)
    {
    }
};

struct RegisterListenerResponse final : public Response {
    RegisterListenerResponse()
        : Response(Response::Type::RegisterListener)
    {
    }
};

struct UnregisterListenerResponse final : public Response {
    UnregisterListenerResponse()
        : Response(Response::Type::UnregisterListener)
    {
    }
};

struct GameChangedResponse final : public Response {
    GameChangedResponse()
        : Response(Response::Type::GameChanged)
    {
    }
};

} // namespace k10engine::Server

template<>
void encode(ByteBuffer&, const k10engine::Server::Response::Type&);

template<>
void encode(ByteBuffer&, const k10engine::Server::Response::ErrorCode&);

template<>
bool decode(ByteBuffer&, k10engine::Server::Response::Type&);

template<>
void encode(ByteBuffer&, const k10engine::Server::Response&);

template<>
bool decode(ByteBuffer&, k10engine::Server::Response&);

template<>
void encode(ByteBuffer&, const k10engine::Server::ErrorResponse&);

template<>
void encode(ByteBuffer&, const k10engine::Server::RegisterUserResponse&);

template<>
bool decode(ByteBuffer&, k10engine::Server::RegisterUserResponse&);

template<>
void encode(ByteBuffer&, const k10engine::Server::NewGameResponse&);

template<>
void encode(ByteBuffer&, const k10engine::Server::JoinGameResponse&);

template<>
void encode(ByteBuffer&, const k10engine::Server::LeaveGameResponse&);

template<>
void encode(ByteBuffer&, const k10engine::Server::StartGameResponse&);
