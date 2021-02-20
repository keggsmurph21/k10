#include <cstring>
#include <iostream>
#include <unistd.h>

#include "Util/ServerBase.h"

class TestServer final : public ServerBase {
public:
    TestServer(int port, int listen_sock)
        : ServerBase(port, listen_sock)
    {
    }

private:
    [[nodiscard]] bool on_connect(int fd, std::string) override;
    [[nodiscard]] bool on_read(int fd, ByteBuffer&) override;
    [[nodiscard]] bool on_writeable(int fd) override;
    [[nodiscard]] bool on_disconnect(int fd) override;
};

bool TestServer::on_connect(int, std::string)
{
    return true;
}

bool TestServer::on_read(int, ByteBuffer& request)
{
    (void)this;
    (void)request;
    return true;
}

bool TestServer::on_writeable(int fd)
{
    ByteBuffer buf{ "hello", 5 };
    return send(fd, buf);
}

bool TestServer::on_disconnect(int)
{
    return true;
}

int main(int argc, char** argv)
{
    if (argc == 1) {
        std::cerr << "Missing required arg: <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);

    auto* server = ServerBase::create<TestServer>(port);
    if (server == nullptr) {
        std::cerr << "Unable to listen on port " << port << std::endl;
        return 1;
    }

    server->serve();
}
