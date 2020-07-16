#include <cassert>
#include <iomanip>
#include <unistd.h>

#include "Server/Server.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"

namespace k10engine::Server {

Server::Server(int port, int listen_sock, std::string game_cache_path, size_t game_cache_size)
    : ServerBase(port, listen_sock)
    , m_game_cache(GameCache(std::move(game_cache_path), game_cache_size))
{
}

bool Server::on_connect(int fd, char* client_ip)
{
    (void)fd;
    (void)client_ip;
    return true;
}

bool Server::on_read(int fd, ByteBuffer& buf)
{
    Decoder decoder(buf);

    Request* request = nullptr;
    if (!decoder.decode(request))
        assert(false);
    auto* response = handle(request);
    delete request;

    if (response == nullptr)
        assert(false);

    buf.clear();
    Encoder encoder(buf);
    encoder << *response;
    delete response;
    std::cout << "response bytes: " << buf << std::endl;

    if (::write(fd, buf.data(), buf.unread_size()) < 0) {
        if (errno == EPIPE) {
            std::cerr << "ignoring broken pipe..." << std::endl;
            return true;
        }
        perror("write");
        return false;
    }
    ::close(fd);

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

const RegisterUserResponse* Server::handle_register_user(const RegisterUserRequest* request)
{
    const auto registration = m_registrar.register_user(request->m_name, request->m_secret);
    if (registration.has_value()) {
        std::cout << "Registered player_id: " << registration->player_id
                  << ", player_secret: " << registration->internal_secret << std::endl;
    }
    return new RegisterUserResponse(registration);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const NewGameResponse* Server::handle_new_game(const NewGameRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const JoinGameResponse* Server::handle_join_game(const JoinGameRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const LeaveGameResponse* Server::handle_leave_game(const LeaveGameRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const StartGameResponse* Server::handle_start_game(const StartGameRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const MakeMoveResponse* Server::handle_make_move(const MakeMoveRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const QueryResponse* Server::handle_query(const QueryRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const RegisterListenerResponse* Server::handle_register_listener(const RegisterListenerRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const UnregisterListenerResponse* Server::handle_unregister_listener(const UnregisterListenerRequest* request)
{
    (void)request;
    assert(false);
}

} // namespace k10engine::Server
