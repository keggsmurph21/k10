#include <cassert>

#include "Server/Response.h"

namespace k10engine::Server {

} // namespace k10engine::Server

template<>
void encode(ByteBuffer& buf, const k10engine::Server::Response::Type& type)
{
    Encoder encoder(buf);
    encoder << static_cast<u8>(type);
}

template<>
bool decode(ByteBuffer& buf, k10engine::Server::Response::Type& type)
{
    if (buf.unread_size() < 1)
        return false;
    u8 enum_value = buf.read();
    if (enum_value > static_cast<u8>(k10engine::Server::Response::Type::GameChanged))
        return false;
    type = static_cast<k10engine::Server::Response::Type>(enum_value);
    return true;
}

template<>
void encode(ByteBuffer& buf, const k10engine::Server::Response& response)
{
    Encoder encoder(buf);
    encoder << response.m_request_id;
    encoder << response.m_type;
    switch (response.m_type) {
    case k10engine::Server::Response::Type::RegisterUser:
        encoder << *static_cast<const k10engine::Server::RegisterUserResponse*>(&response);
        break;
    case k10engine::Server::Response::Type::NewGame:
        encoder << *static_cast<const k10engine::Server::NewGameResponse*>(&response);
        break;
    case k10engine::Server::Response::Type::JoinGame:
        encoder << *static_cast<const k10engine::Server::JoinGameResponse*>(&response);
        break;
    case k10engine::Server::Response::Type::LeaveGame:
        encoder << *static_cast<const k10engine::Server::LeaveGameResponse*>(&response);
        break;
    case k10engine::Server::Response::Type::StartGame:
        encoder << *static_cast<const k10engine::Server::StartGameResponse*>(&response);
        break;
    case k10engine::Server::Response::Type::MakeMove:
        encoder << *static_cast<const k10engine::Server::MakeMoveResponse*>(&response);
        break;
    case k10engine::Server::Response::Type::Query:
        encoder << *static_cast<const k10engine::Server::QueryResponse*>(&response);
        break;
    case k10engine::Server::Response::Type::RegisterListener:
        encoder << *static_cast<const k10engine::Server::RegisterListenerResponse*>(&response);
        break;
    case k10engine::Server::Response::Type::UnregisterListener:
        encoder << *static_cast<const k10engine::Server::UnregisterListenerResponse*>(&response);
        break;
    case k10engine::Server::Response::Type::GameChanged:
        encoder << *static_cast<const k10engine::Server::GameChangedResponse*>(&response);
        break;
    default:
        assert(false);
    }
}

template<>
bool decode(ByteBuffer&, k10engine::Server::Response&)
{
    assert(false);
}

template<>
void encode(ByteBuffer& buf, const k10engine::Server::RegisterUserResponse& response)
{
    Encoder encoder(buf);
    encoder << response.m_registration;
}

template<>
bool decode(ByteBuffer&, k10engine::Server::RegisterUserResponse&)
{
    assert(false);
}

template<>
void encode(ByteBuffer& buf, const k10engine::Server::NewGameResponse& response)
{
    Encoder encoder(buf);
    encoder << response.m_game_id;
}

template<>
void encode(ByteBuffer&, const k10engine::Server::JoinGameResponse&)
{
    // Nothing to do here ...
}

template<>
void encode(ByteBuffer&, const k10engine::Server::LeaveGameResponse&)
{
    // Nothing to do here ...
}

template<>
void encode(ByteBuffer&, const k10engine::Server::StartGameResponse&)
{
    // Nothing to do here ...
}
