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

    const Response* handle(const Request&);
};

} // namespace k10engine::Server

