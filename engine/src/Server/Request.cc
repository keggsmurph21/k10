#include <cassert>
#include <cstring>

#include "Server/Request.h"

namespace k10engine::Server {

const Request* Request::decode(Decoder& decoder)
{
    Type request_type;
    if (!decoder.decode(request_type))
        return nullptr;
    switch (request_type) {
    case Type::RegisterUser:
        return RegisterUserRequest::decode(decoder);
    case Type::NewGame:
        return NewGameRequest::decode(decoder);
    case Type::JoinGame:
        return JoinGameRequest::decode(decoder);
    case Type::LeaveGame:
        return LeaveGameRequest::decode(decoder);
    case Type::StartGame:
        return StartGameRequest::decode(decoder);
    case Type::MakeMove:
        return MakeMoveRequest::decode(decoder);
    case Type::Query:
        return QueryRequest::decode(decoder);
    case Type::RegisterListener:
        return RegisterListenerRequest::decode(decoder);
    case Type::UnregisterListener:
        return UnregisterListenerRequest::decode(decoder);
    default:
        return nullptr;
    };
}

const RegisterUserRequest* RegisterUserRequest::decode(Decoder& decoder)
{
    std::string name;
    if (!decoder.decode(name))
        return nullptr;
    if (name.length() == 0)
        return nullptr;

    std::string secret;
    if (!decoder.decode(secret))
        return nullptr;
    if (secret.length() == 0)
        return nullptr;

    return new RegisterUserRequest{ name, secret };
}

const NewGameRequest* NewGameRequest::decode(Decoder& decoder)
{
    (void)decoder;
    assert(false);
}

const JoinGameRequest* JoinGameRequest::decode(Decoder& decoder)
{
    Registrar::PlayerId player_id;
    if (!decoder.decode(player_id))
        return nullptr;

    Registrar::PlayerSecret player_secret;
    if (!decoder.decode(player_secret))
        return nullptr;

    GameId game_id;
    if (!decoder.decode(game_id))
        return nullptr;

    return new JoinGameRequest{ player_id, player_secret, game_id };
}

const LeaveGameRequest* LeaveGameRequest::decode(Decoder& decoder)
{
    (void)decoder;
    assert(false);
}

const StartGameRequest* StartGameRequest::decode(Decoder& decoder)
{
    (void)decoder;
    assert(false);
}

const MakeMoveRequest* MakeMoveRequest::decode(Decoder& decoder)
{
    (void)decoder;
    assert(false);
}

const QueryRequest* QueryRequest::decode(Decoder& decoder)
{
    (void)decoder;
    assert(false);
}

const RegisterListenerRequest* RegisterListenerRequest::decode(Decoder& decoder)
{
    (void)decoder;
    assert(false);
}

const UnregisterListenerRequest* UnregisterListenerRequest::decode(Decoder& decoder)
{
    (void)decoder;
    assert(false);
}

} // namespace k10engine::Server

template<>
void encode(ByteBuffer& buf, k10engine::Server::Request::Type& type)
{
    buf.append(static_cast<u8>(type));
}

template<>
bool decode(ByteBuffer& buf, k10engine::Server::Request::Type& type)
{
    if (buf.unread_size() < 1)
        return false;
    u8 enum_value = buf.read();
    if (enum_value > static_cast<u8>(k10engine::Server::Request::Type::UnregisterListener))
        return false;
    type = static_cast<k10engine::Server::Request::Type>(enum_value);
    return true;
}

