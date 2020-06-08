#include <cassert>
#include <cstring>

#include "Server/Request.h"

namespace k10engine::Server {
} // namespace k10engine::Server

using Request = k10engine::Server::Request;
using RegisterUserRequest = k10engine::Server::RegisterUserRequest;
// using NewGameRequest = k10engine::Server::NewGameRequest;
using JoinGameRequest = k10engine::Server::JoinGameRequest;
// using LeaveGameRequest = k10engine::Server::LeaveGameRequest;
// using StartGameRequest = k10engine::Server::StartGameRequest;
// using MakeMoveRequest = k10engine::Server::MakeMoveRequest;
// using QueryRequest = k10engine::Server::QueryRequest;
// using RegisterListenerRequest = k10engine::Server::RegisterListenerRequest;
// using UnregisterListenerRequest = k10engine::Server::UnregisterListenerRequest;

template<>
void encode(ByteBuffer& buf, const Request::Type& type)
{
    Encoder encoder(buf);
    encoder << static_cast<u8>(type);
}

template<>
bool decode(ByteBuffer& buf, Request::Type& type)
{
    Decoder decoder(buf);
    u8 byte;
    if (!decoder.decode(byte))
        return false;
    if (byte > static_cast<u8>(Request::Type::UnregisterListener))
        return false;
    type = static_cast<Request::Type>(byte);
    return true;
}

template<>
bool decode(ByteBuffer& buf, Request*& request)
{
    Decoder decoder(buf);
    Request::Type request_type;
    if (!decoder.decode(request_type)) {
        request = nullptr;
        return false;
    }
    switch (request_type) {
    case Request::Type::RegisterUser: {
        auto* register_user_request = static_cast<RegisterUserRequest*>(request);
        if (!decoder.decode(register_user_request))
            return false;
        request = register_user_request;
        return true;
    }
    case Request::Type::NewGame:
        request = nullptr;
        return false;
    case Request::Type::JoinGame: {
        auto* join_game_request = static_cast<JoinGameRequest*>(request);
        if (!decoder.decode(join_game_request))
            return false;
        request = join_game_request;
        return true;
    }
    case Request::Type::LeaveGame:
        request = nullptr;
        return false;
    case Request::Type::StartGame:
        request = nullptr;
        return false;
    case Request::Type::MakeMove:
        request = nullptr;
        return false;
    case Request::Type::Query:
        request = nullptr;
        return false;
    case Request::Type::RegisterListener:
        request = nullptr;
        return false;
    case Request::Type::UnregisterListener:
        request = nullptr;
        return false;
    default:
        request = nullptr;
        return false;
    };
}

template<>
bool decode(ByteBuffer& buf, k10engine::Server::RegisterUserRequest*& request)
{
    Decoder decoder(buf);

    std::string name;
    if (!decoder.decode(name))
        return false;
    if (name.length() == 0)
        return false;

    std::string secret;
    if (!decoder.decode(secret))
        return false;
    if (secret.length() == 0)
        return false;

    request = new RegisterUserRequest{ name, secret };
    return true;
}

template<>
bool decode(ByteBuffer& buf, k10engine::Server::JoinGameRequest*& request)
{
    Decoder decoder(buf);

    k10engine::Server::Registrar::PlayerId player_id;
    if (!decoder.decode(player_id))
        return false;

    k10engine::Server::Registrar::PlayerSecret player_secret;
    if (!decoder.decode(player_secret))
        return false;

    k10engine::Server::GameId game_id;
    if (!decoder.decode(game_id))
        return false;

    request = new JoinGameRequest{ player_id, player_secret, game_id };
    return true;
}
