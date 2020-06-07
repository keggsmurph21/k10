#pragma once

#include "Server/Registrar.h"
#include "Server/Request.h"
#include "Server/Response.h"
#include "Util/ServerBase.h"

namespace k10engine::Server {

class Server final : public ServerBase {
public:
    Server(int port, int listen_sock)
        : ServerBase(port, listen_sock)
    {
    }

private:
    [[nodiscard]] bool on_connect(int fd, char* client_ip) override;
    [[nodiscard]] bool on_read(int fd, char* buf, int len) override;

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
};

} // namespace k10engine::Server

