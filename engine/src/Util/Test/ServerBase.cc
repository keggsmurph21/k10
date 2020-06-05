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
    [[nodiscard]] bool on_connect(int, char*) override { return true; }
    [[nodiscard]] bool on_read(int fd, char* buf, int len) override;
};

bool TestServer::on_read(int fd, char* buf, int len)
{
    (void)len; // not reading from buf

    std::memset(buf, 0, 5);
    std::strncpy(buf, "ack\n", 5);

    if (write(fd, buf, strlen(buf)) < 0) {
        if (errno == EPIPE) {
            std::cerr << "ignoring broken pipe..." << std::endl;
            return true;
        }
        perror("write");
        return false;
    }

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
