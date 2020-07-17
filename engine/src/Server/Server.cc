#include <cassert>
#include <iomanip>
#include <unistd.h>

#include "Server/Server.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"

namespace k10engine::Server {

#define VALIDATE_REQUEST(request)                                                     \
    if (!m_registrar.validate_player(request->m_player_id, request->m_player_secret)) \
        assert(false);

Server::Server(int port, int listen_sock, std::string game_cache_path, size_t game_cache_size)
    : ServerBase(port, listen_sock)
    , m_game_cache(GameCache(std::move(game_cache_path), game_cache_size))
{
    // We want to make sure we get the registration for player id 0!
    (void)m_registrar.register_user("admin", "what is beadly");

    // Maybe there's a better initial number than 256?
    m_clients.reserve(256);
    for (auto i = 0; i < 256; i++)
        m_clients.push_back(nullptr);
}

Server::~Server()
{
    for (auto* client : m_clients)
        delete client;
}

bool Server::on_connect(int fd, std::string client_ip)
{
    m_clients[fd] = new Client(fd, std::move(client_ip));
    return true;
}

void Server::for_each_interested_client(Game::GameId game_id, std::function<void(Client*)> callback)
{
    for (auto* client : m_clients) {
        if (client && client->is_interested_in(game_id))
            callback(client);
    }
}

bool Server::on_read(int fd, ByteBuffer& buf)
{
    Client* client = m_clients.at(fd);

    Decoder decoder(buf);

    Request* request = nullptr;
    if (!decoder.decode(request))
        assert(false);

    request->m_client_id = client->id();

    auto* response = handle(request);
    delete request;

    if (response == nullptr)
        assert(false);

    client->queue_response(response);

    return true;
}

bool Server::on_writeable(int fd)
{
    ByteBuffer buf;
    Client* client = m_clients.at(fd);

    return client->for_each_pending_response([&](const Response* response) -> bool {
        buf.clear();
        Encoder encoder(buf);
        encoder << *response;
        return send(fd, buf);
    });
}

bool Server::on_disconnect(int fd)
{
    Client* client = m_clients.at(fd);
    delete client;
    return true;
}

const Response* Server::handle(const Request* request)
{
    assert(request != nullptr);
    Response* response = nullptr;
    switch (request->m_type) {
    case Request::Type::RegisterUser:
        response = handle_register_user(static_cast<const RegisterUserRequest*>(request));
        break;
    case Request::Type::NewGame:
        response = handle_new_game(static_cast<const NewGameRequest*>(request));
        break;
    case Request::Type::JoinGame:
        response = handle_join_game(static_cast<const JoinGameRequest*>(request));
        break;
    case Request::Type::LeaveGame:
        response = handle_leave_game(static_cast<const LeaveGameRequest*>(request));
        break;
    case Request::Type::StartGame:
        response = handle_start_game(static_cast<const StartGameRequest*>(request));
        break;
    case Request::Type::MakeMove:
        response = handle_make_move(static_cast<const MakeMoveRequest*>(request));
        break;
    case Request::Type::Query:
        response = handle_query(static_cast<const QueryRequest*>(request));
        break;
    case Request::Type::RegisterListener:
        response = handle_register_listener(static_cast<const RegisterListenerRequest*>(request));
        break;
    case Request::Type::UnregisterListener:
        response = handle_unregister_listener(static_cast<const UnregisterListenerRequest*>(request));
        break;
    default:
        assert(false);
    }
    if (response) {
        response->m_client_id = request->m_client_id;
        response->m_request_id = request->m_id;
    }
    return response;
}

RegisterUserResponse* Server::handle_register_user(const RegisterUserRequest* request)
{
    const auto registration = m_registrar.register_user(request->m_name, request->m_secret);
    if (registration.has_value()) {
        std::cout << "Registered player_id: " << registration->player_id
                  << ", player_secret: " << registration->internal_secret << std::endl;
    }
    return new RegisterUserResponse(registration);
}

NewGameResponse* Server::handle_new_game(const NewGameRequest* request)
{
    VALIDATE_REQUEST(request);
    auto* game = Game::Game::initialize(request->m_player_id, request->m_scenario, request->m_parameters);
    if (game == nullptr)
        assert(false);
    auto game_id = m_game_cache.insert(game);
    return new NewGameResponse(game_id);
}

JoinGameResponse* Server::handle_join_game(const JoinGameRequest* request)
{
    VALIDATE_REQUEST(request);
    auto* game = m_game_cache.get(request->m_game_id);
    if (game == nullptr)
        assert(false);
    if (!game->join(request->m_player_id))
        assert(false);
    return new JoinGameResponse();
}

LeaveGameResponse* Server::handle_leave_game(const LeaveGameRequest* request)
{
    VALIDATE_REQUEST(request);
    auto* game = m_game_cache.get(request->m_game_id);
    if (game == nullptr)
        assert(false);
    if (!game->leave(request->m_player_id))
        assert(false);
    return new LeaveGameResponse();
}

StartGameResponse* Server::handle_start_game(const StartGameRequest* request)
{
    VALIDATE_REQUEST(request);
    auto* game = m_game_cache.get(request->m_game_id);
    if (game == nullptr)
        assert(false);
    if (!game->start())
        assert(false);
    return new StartGameResponse();
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
MakeMoveResponse* Server::handle_make_move(const MakeMoveRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
QueryResponse* Server::handle_query(const QueryRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
RegisterListenerResponse* Server::handle_register_listener(const RegisterListenerRequest* request)
{
    (void)request;
    assert(false);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
UnregisterListenerResponse* Server::handle_unregister_listener(const UnregisterListenerRequest* request)
{
    (void)request;
    assert(false);
}

} // namespace k10engine::Server
