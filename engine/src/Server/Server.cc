#include <arpa/inet.h>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <unistd.h>

#include "Server/Server.h"

namespace k10engine::Server {

Server::Server(int port, int listen_sock)
    : m_port(port)
    , m_listen_sock(listen_sock)
{
    std::cout << "Listening on port " << port << std::endl;
}

Server* Server::create(int port)
{
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        perror("socket");
        return nullptr;
    }

    // Note: on BSDs, you can just setsockopt(SO_NOSIGPIPE) :^)
    int enable = 1;
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        perror("setsockopt(SO_REUSEADDR)");
        return nullptr;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    socklen_t addrlen = sizeof(server_addr);

    if (bind(listen_sock, reinterpret_cast<sockaddr*>(&server_addr), addrlen) < 0) {
        perror("bind");
        return nullptr;
    }

    if (listen(listen_sock, MAX_QUEUED_CONNECTIONS) < 0) {
        perror("listen");
        return nullptr;
    }

    auto* server = new Server(port, listen_sock);

    struct sigaction ignore_sigpipe;
    ignore_sigpipe.sa_handler = SIG_IGN;
    sigemptyset(&ignore_sigpipe.sa_mask);
    sigaction(SIGPIPE, &ignore_sigpipe, nullptr);

    return server;
}

void Server::serve()
{
    m_epoll_fd = epoll_create1(0);
    if (m_epoll_fd < 0) {
        perror("epoll_create1");
        exit(1);
    }

    struct epoll_event event;
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    char buf[256];

    event.events = EPOLLIN;
    event.data.fd = m_listen_sock;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_listen_sock, &event) != 0) {
        perror("epoll_ctl");
        exit(1);
    }

    std::cout << "epoll fd: " << m_listen_sock << std::endl;

    for (;;) {
        int num_fds = epoll_wait(m_epoll_fd, m_events, MAX_QUEUED_EPOLL_EVENTS, -1); // no timeout
        if (num_fds == -1) {
            perror("epoll_wait");
            exit(1);
        }

        std::cout << std::endl << "looping thru " << num_fds << " events ..." << std::endl;
        for (int i = 0; i < num_fds; ++i) {
            auto event_fd = m_events[i].data.fd;
            std::cout << "handling event(" << i << ") with fd " << event_fd << std::endl;
            if (event_fd == m_listen_sock) {
                int conn_sock = accept(m_listen_sock, reinterpret_cast<sockaddr*>(&client_addr), &addrlen);
                if (conn_sock == -1) {
                    perror("accept");
                    exit(1);
                }
                event.events = EPOLLIN;
                event.data.fd = conn_sock;
                if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, conn_sock, &event) == -1) {
                    perror("epoll_ctl");
                    exit(1);
                }
                auto client_ip_addr = client_addr.sin_addr;
                char client_ip[INET_ADDRSTRLEN];
                if (inet_ntop(AF_INET, &client_ip_addr, client_ip, INET_ADDRSTRLEN) == nullptr) {
                    perror("inet_ntop");
                    exit(1);
                }
                on_connect(conn_sock, client_ip);
            } else {
                int bytes_read = read(event_fd, buf, sizeof(buf) - 1);
                if (bytes_read < 0) {
                    perror("read");
                    exit(1);
                }
                on_read(event_fd, buf, bytes_read);
            }
        }
    }
}

void Server::on_connect(int fd, char* client_ip) // NOLINT(readability-convert-member-functions-to-static)
{
    std::cout << "Got a new connection from " << client_ip << " on fd " << fd << std::endl;
}

void Server::on_read(int fd, char* buf, int len) // NOLINT(readability-convert-member-functions-to-static)
{
    std::cout << "req(" << len << "):";
    for (int i = 0; i < len; ++i) {
        std::cout << " " << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(buf[i]);
    }
    std::cout << " [" << buf << "]" << std::endl;

    if (len == 0) {
        std::cout << "closing fd " << fd << std::endl;
        ::close(fd);
        return;
    }

    std::memset(buf, 0, 5);
    std::strncpy(buf, "ack\n", 5);

    if (write(fd, buf, strlen(buf)) < 0) {
        if (errno == EPIPE) {
            std::cerr << "ignoring broken pipe..." << std::endl;
            return;
        }
        perror("write");
        exit(1);
    }
}

Server::~Server()
{
    close();
}

void Server::close()
{
    ::close(m_epoll_fd);
    ::close(m_listen_sock);
}

} // namespace k10engine::Server
