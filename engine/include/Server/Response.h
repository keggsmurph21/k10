#pragma once

#include "Registrar.h"
#include "Util/ByteBuffer.h"
#include "Util/Types.h"

namespace k10engine::Server {

struct Response {
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
        GameChanged,
    };

    const Type m_type;

    virtual void encode(ByteBuffer&) const;

    virtual ~Response() {}

protected:
    Response(Type type)
        : m_type(type)
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

    void encode(ByteBuffer&) const override;
};

struct NewGameResponse final : public Response {
    NewGameResponse()
        : Response(Response::Type::NewGame)
    {
    }

    void encode(ByteBuffer&) const override;
};

struct JoinGameResponse final : public Response {
    JoinGameResponse()
        : Response(Response::Type::JoinGame)
    {
    }

    void encode(ByteBuffer&) const override;
};

struct LeaveGameResponse final : public Response {
    LeaveGameResponse()
        : Response(Response::Type::LeaveGame)
    {
    }

    void encode(ByteBuffer&) const override;
};

struct StartGameResponse final : public Response {
    StartGameResponse()
        : Response(Response::Type::StartGame)
    {
    }

    void encode(ByteBuffer&) const override;
};

struct MakeMoveResponse final : public Response {
    MakeMoveResponse()
        : Response(Response::Type::MakeMove)
    {
    }

    void encode(ByteBuffer&) const override;
};

struct QueryResponse final : public Response {
    QueryResponse()
        : Response(Response::Type::Query)
    {
    }

    void encode(ByteBuffer&) const override;
};

struct RegisterListenerResponse final : public Response {
    RegisterListenerResponse()
        : Response(Response::Type::RegisterListener)
    {
    }

    void encode(ByteBuffer&) const override;
};

struct UnregisterListenerResponse final : public Response {
    UnregisterListenerResponse()
        : Response(Response::Type::UnregisterListener)
    {
    }

    void encode(ByteBuffer&) const override;
};

struct GameChangedResponse final : public Response {
    GameChangedResponse()
        : Response(Response::Type::GameChanged)
    {
    }

    void encode(ByteBuffer&) const override;
};

} // namespace k10engine::Server
