#include <iostream>

#include "Server/Server.h"

int main(int argc, char** argv)
{
    if (argc == 1) {
        std::cerr << "Missing required arg: <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);

    auto* server = k10engine::Server::Server::create(port);
    if (server == nullptr) {
        std::cerr << "Unable to listen on port " << port << std::endl;
        return 1;
    }

    server->serve();
}
