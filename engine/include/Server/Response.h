#pragma once

#include "Registrar.h"

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

    virtual ~Response() {}

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
