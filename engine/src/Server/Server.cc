#include <cassert>

#include "Server/Server.h"

namespace k10engine::Server {

bool Server::on_connect(int fd, char* client_ip)
{
    (void)fd;
    (void)client_ip;
    return true;
}

bool Server::on_read(int fd, char* buf, int len)
{
    // FIXME: Clean up the Request* and Response* !
    (void)fd;
    auto* request = Request::parse(buf, len);
    if (request == nullptr)
        assert(false);
    auto* response = handle(request);
    delete request;
    if (response == nullptr)
        assert(false);
    delete response;
    return true;
}

const Response* Server::handle(const Request* request)
{
    assert(request != nullptr);
    switch (request->m_type) {
    case Request::Type::RegisterUser:
        return handle_register_user(static_cast<const RegisterUserRequest*>(request));
    case Request::Type::NewGame:
        return handle_new_game(static_cast<const NewGameRequest*>(request));
    case Request::Type::JoinGame:
        return handle_join_game(static_cast<const JoinGameRequest*>(request));
    case Request::Type::LeaveGame:
        return handle_leave_game(static_cast<const LeaveGameRequest*>(request));
    case Request::Type::StartGame:
        return handle_start_game(static_cast<const StartGameRequest*>(request));
    case Request::Type::MakeMove:
        return handle_make_move(static_cast<const MakeMoveRequest*>(request));
    case Request::Type::Query:
        return handle_query(static_cast<const QueryRequest*>(request));
    case Request::Type::RegisterListener:
        return handle_register_listener(static_cast<const RegisterListenerRequest*>(request));
    case Request::Type::UnregisterListener:
        return handle_unregister_listener(static_cast<const UnregisterListenerRequest*>(request));
    default:
        assert(false);
    }
    return nullptr;
}

const Response* Server::handle_register_user(const RegisterUserRequest* request)
{
    const auto registration = m_registrar.register_user(
        request->m_name.c_str(), request->m_name.size(), request->m_secret.c_str(), request->m_secret.size());
    assert(registration.has_value());
    std::cout << "Registered player_id: " << registration->player_id
              << ", player_secret: " << registration->internal_secret << std::endl;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const Response* Server::handle_new_game(const NewGameRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const Response* Server::handle_join_game(const JoinGameRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const Response* Server::handle_leave_game(const LeaveGameRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const Response* Server::handle_start_game(const StartGameRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const Response* Server::handle_make_move(const MakeMoveRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const Response* Server::handle_query(const QueryRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const Response* Server::handle_register_listener(const RegisterListenerRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const Response* Server::handle_unregister_listener(const UnregisterListenerRequest* request)
{
    (void)request;
    assert(false);
}

} // namespace k10engine::Server
