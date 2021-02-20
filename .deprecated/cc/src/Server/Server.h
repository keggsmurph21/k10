#pragma once

#include <vector>

#include "Server/Client.h"
#include "Server/GameCache.h"
#include "Server/Registrar.h"
#include "Server/Request.h"
#include "Server/Response.h"
#include "Util/ByteBuffer.h"
#include "Util/ServerBase.h"

namespace k10engine::Server {

class Server final : public ServerBase {
public:
    Server(int port, int listen_sock, std::string game_cache_path, size_t game_cache_size);
    ~Server();

private:
    [[nodiscard]] bool on_connect(int fd, std::string client_ip) override;
    [[nodiscard]] bool on_read(int fd, ByteBuffer&) override;
    [[nodiscard]] bool on_writeable(int fd) override;
    [[nodiscard]] bool on_disconnect(int fd) override;

    void for_each_interested_client(Game::GameId, std::function<void(Client*)>);

    Response* handle(const Request*);
    Response* handle_register_user(const RegisterUserRequest*);
    Response* handle_new_game(const NewGameRequest*);
    Response* handle_join_game(const JoinGameRequest*);
    Response* handle_leave_game(const LeaveGameRequest*);
    Response* handle_start_game(const StartGameRequest*);
    Response* handle_make_move(const MakeMoveRequest*);
    Response* handle_query(const QueryRequest*);
    Response* handle_register_listener(const RegisterListenerRequest*);
    Response* handle_unregister_listener(const UnregisterListenerRequest*);

    Registrar m_registrar;
    GameCache m_game_cache;

    std::vector<Client*> m_clients;
};

} // namespace k10engine::Server

