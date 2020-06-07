#pragma once

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
    const Response* handle_register_user(const RegisterUserRequest*);
    const Response* handle_new_game(const NewGameRequest*);
    const Response* handle_join_game(const JoinGameRequest*);
    const Response* handle_leave_game(const LeaveGameRequest*);
    const Response* handle_start_game(const StartGameRequest*);
    const Response* handle_make_move(const MakeMoveRequest*);
    const Response* handle_query(const QueryRequest*);
    const Response* handle_register_listener(const RegisterListenerRequest*);
    const Response* handle_unregister_listener(const UnregisterListenerRequest*);
};

} // namespace k10engine::Server

