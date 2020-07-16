#pragma once

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

private:
    [[nodiscard]] bool on_connect(int fd, std::string client_ip) override;
    [[nodiscard]] bool on_read(ByteBuffer&) override;
    [[nodiscard]] bool on_writeable(int fd) override;

    const Response* handle(const Request*);
    const RegisterUserResponse* handle_register_user(const RegisterUserRequest*);
    const NewGameResponse* handle_new_game(const NewGameRequest*);
    const JoinGameResponse* handle_join_game(const JoinGameRequest*);
    const LeaveGameResponse* handle_leave_game(const LeaveGameRequest*);
    const StartGameResponse* handle_start_game(const StartGameRequest*);
    const MakeMoveResponse* handle_make_move(const MakeMoveRequest*);
    const QueryResponse* handle_query(const QueryRequest*);
    const RegisterListenerResponse* handle_register_listener(const RegisterListenerRequest*);
    const UnregisterListenerResponse* handle_unregister_listener(const UnregisterListenerRequest*);

    Registrar m_registrar;
    GameCache m_game_cache;
};

} // namespace k10engine::Server

