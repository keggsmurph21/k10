#include <cassert>

#include "Server/Server.h"

namespace k10engine::Server {

bool Server::on_connect(int fd, char* client_ip)
{
    (void)fd;
    (void)client_ip;
    return true;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
const Response* Server::handle(const Request& request)
{
    (void)request;
    assert(false);
}

bool Server::on_read(int fd, char* buf, int len)
{
    // FIXME: Clean up the Request* and Response* !
    (void)fd;
    auto* request = Request::parse(buf, len);
    if (request == nullptr)
        assert(false);
    auto* response = handle(*request);
    if (response == nullptr)
        assert(false);
    return true;
}

} // namespace k10engine::Server
