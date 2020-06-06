#include <cassert>
#include <cstring>

#include "Server/Request.h"

namespace k10engine::Server {

const Request* Request::parse(const char* buf, int len)
{
    assert(buf != nullptr);
    if (len == 0)
        return nullptr;
    Type request_type = static_cast<Type>(*buf++);
    switch (request_type) {
    case Type::RegisterUser:
        return RegisterUserRequest::parse(buf, len - 1);
    case Type::NewGame:
        return NewGameRequest::parse(buf, len - 1);
    case Type::JoinGame:
        return JoinGameRequest::parse(buf, len - 1);
    case Type::LeaveGame:
        return LeaveGameRequest::parse(buf, len - 1);
    case Type::StartGame:
        return StartGameRequest::parse(buf, len - 1);
    case Type::MakeMove:
        return MakeMoveRequest::parse(buf, len - 1);
    case Type::Query:
        return QueryRequest::parse(buf, len - 1);
    case Type::RegisterListener:
        return RegisterListenerRequest::parse(buf, len - 1);
    case Type::UnregisterListener:
        return UnregisterListenerRequest::parse(buf, len - 1);
    default:
        return nullptr;
    };
}

const RegisterUserRequest* RegisterUserRequest::parse(const char* buf, int len)
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

const NewGameRequest* NewGameRequest::parse(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

const JoinGameRequest* JoinGameRequest::parse(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

const LeaveGameRequest* LeaveGameRequest::parse(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

const StartGameRequest* StartGameRequest::parse(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

const MakeMoveRequest* MakeMoveRequest::parse(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

const QueryRequest* QueryRequest::parse(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

const RegisterListenerRequest* RegisterListenerRequest::parse(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

const UnregisterListenerRequest* UnregisterListenerRequest::parse(const char* buf, int len)
{
    (void)buf;
    (void)len;
    assert(false);
}

} // namespace k10engine::Server
