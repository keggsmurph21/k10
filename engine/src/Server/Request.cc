#include <cassert>
#include <cstring>

#include "Server/Request.h"

namespace k10engine::Server {

const Request* Request::decode(const char* buf, int len)
{
    assert(buf != nullptr);
    if (len == 0)
        return nullptr;
    auto request_type = static_cast<Type>(*buf++);
    switch (request_type) {
    case Type::RegisterUser:
        return RegisterUserRequest::decode(buf, len - 1);
    case Type::NewGame:
        return NewGameRequest::decode(buf, len - 1);
    case Type::JoinGame:
        return JoinGameRequest::decode(buf, len - 1);
    case Type::LeaveGame:
        return LeaveGameRequest::decode(buf, len - 1);
    case Type::StartGame:
        return StartGameRequest::decode(buf, len - 1);
    case Type::MakeMove:
        return MakeMoveRequest::decode(buf, len - 1);
    case Type::Query:
        return QueryRequest::decode(buf, len - 1);
    case Type::RegisterListener:
        return RegisterListenerRequest::decode(buf, len - 1);
    case Type::UnregisterListener:
        return UnregisterListenerRequest::decode(buf, len - 1);
    default:
        return nullptr;
    };
}

const RegisterUserRequest* RegisterUserRequest::decode(const char* buf, int len)
{
    static char name_buf[255];
    static char secret_buf[255];

    // can do stricter size-checking up front
    if (len < 1)
        return nullptr;

    // FIXME: Enforce a better minimum len here (probably in Registrar?)
    u8 name_len = *buf++;
    if (name_len < 1)
        return nullptr;
    if (len < name_len + 1)
        return nullptr; // Shenanigans!

    std::memset(name_buf, 0, 255);
    char* name = name_buf;
    for (auto i = 0; i < name_len; ++i) {
        if ((*name++ = *buf++) == '\0')
            return {};
    }

    // FIXME: See above
    u8 secret_len = *buf++;
    if (secret_len < 1)
        return nullptr;
    if (len < name_len + secret_len + 2)
        return nullptr; // Shenanigans!

    std::memset(secret_buf, 0, 255);
    char* secret = secret_buf;
    for (auto i = 0; i < secret_len; ++i) {
        if ((*secret++ = *buf++) == '\0')
            return {};
    }

    return new RegisterUserRequest{ std::string(name_buf, name_buf + name_len),
                                    std::string(secret_buf, secret_buf + secret_len) };
}

const NewGameRequest* NewGameRequest::decode(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

const JoinGameRequest* JoinGameRequest::decode(const char* buf, int len)
{
    if (static_cast<unsigned int>(len)
        != sizeof(Registrar::PlayerId) + sizeof(Registrar::PlayerSecret) + sizeof(GameId))
        return nullptr;

    Registrar::PlayerId player_id = static_cast<u8>(*buf++);
    player_id = (player_id << 8) | static_cast<u8>(*buf++);
    player_id = (player_id << 8) | static_cast<u8>(*buf++);
    player_id = (player_id << 8) | static_cast<u8>(*buf++);
    player_id = (player_id << 8) | static_cast<u8>(*buf++);
    player_id = (player_id << 8) | static_cast<u8>(*buf++);
    player_id = (player_id << 8) | static_cast<u8>(*buf++);
    player_id = (player_id << 8) | static_cast<u8>(*buf++);

    Registrar::PlayerSecret player_secret = static_cast<u8>(*buf++);
    player_secret = (player_secret << 8) | static_cast<u8>(*buf++);
    player_secret = (player_secret << 8) | static_cast<u8>(*buf++);
    player_secret = (player_secret << 8) | static_cast<u8>(*buf++);
    player_secret = (player_secret << 8) | static_cast<u8>(*buf++);
    player_secret = (player_secret << 8) | static_cast<u8>(*buf++);
    player_secret = (player_secret << 8) | static_cast<u8>(*buf++);
    player_secret = (player_secret << 8) | static_cast<u8>(*buf++);

    GameId game_id = static_cast<u8>(*buf++);
    game_id = (game_id << 8) | static_cast<u8>(*buf++);
    game_id = (game_id << 8) | static_cast<u8>(*buf++);
    game_id = (game_id << 8) | static_cast<u8>(*buf++);
    game_id = (game_id << 8) | static_cast<u8>(*buf++);
    game_id = (game_id << 8) | static_cast<u8>(*buf++);
    game_id = (game_id << 8) | static_cast<u8>(*buf++);
    game_id = (game_id << 8) | static_cast<u8>(*buf++);

    return new JoinGameRequest{ player_id, player_secret, game_id };
}

const LeaveGameRequest* LeaveGameRequest::decode(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

const StartGameRequest* StartGameRequest::decode(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

const MakeMoveRequest* MakeMoveRequest::decode(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

const QueryRequest* QueryRequest::decode(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

const RegisterListenerRequest* RegisterListenerRequest::decode(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

const UnregisterListenerRequest* UnregisterListenerRequest::decode(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

} // namespace k10engine::Server
