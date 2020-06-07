#pragma once

#include "Server/Registrar.h"
#include "Util/ByteBuffer.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"
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

    virtual void encode(Encoder&) const;

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

    void encode(Encoder&) const override;
};

struct NewGameResponse final : public Response {
    NewGameResponse()
        : Response(Response::Type::NewGame)
    {
    }

    void encode(Encoder&) const override;
};

struct JoinGameResponse final : public Response {
    JoinGameResponse()
        : Response(Response::Type::JoinGame)
    {
    }

    void encode(Encoder&) const override;
};

struct LeaveGameResponse final : public Response {
    LeaveGameResponse()
        : Response(Response::Type::LeaveGame)
    {
    }

    void encode(Encoder&) const override;
};

struct StartGameResponse final : public Response {
    StartGameResponse()
        : Response(Response::Type::StartGame)
    {
    }

    void encode(Encoder&) const override;
};

struct MakeMoveResponse final : public Response {
    MakeMoveResponse()
        : Response(Response::Type::MakeMove)
    {
    }

    void encode(Encoder&) const override;
};

struct QueryResponse final : public Response {
    QueryResponse()
        : Response(Response::Type::Query)
    {
    }

    void encode(Encoder&) const override;
};

struct RegisterListenerResponse final : public Response {
    RegisterListenerResponse()
        : Response(Response::Type::RegisterListener)
    {
    }

    void encode(Encoder&) const override;
};

struct UnregisterListenerResponse final : public Response {
    UnregisterListenerResponse()
        : Response(Response::Type::UnregisterListener)
    {
    }

    void encode(Encoder&) const override;
};

struct GameChangedResponse final : public Response {
    GameChangedResponse()
        : Response(Response::Type::GameChanged)
    {
    }

    void encode(Encoder&) const override;
};

} // namespace k10engine::Server

template<>
void encode(ByteBuffer&, k10engine::Server::Response::Type&);

template<>
void encode(ByteBuffer&, const k10engine::Server::Response::Type&);

template<>
bool decode(ByteBuffer&, k10engine::Server::Response::Type&);
