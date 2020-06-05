#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <unistd.h>

#include "Util/ServerBase.h"

ServerBase::ServerBase(int port, int listen_sock)
    : m_port(port)
    , m_listen_sock(listen_sock)
{
    std::cout << "Listening on port " << port << std::endl;

    struct sigaction ignore_sigpipe;
    ignore_sigpipe.sa_handler = SIG_IGN;
    sigemptyset(&ignore_sigpipe.sa_mask);
    sigaction(SIGPIPE, &ignore_sigpipe, nullptr);
}

int ServerBase::bind_and_listen(int port)
{
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        perror("socket");
        return -1;
    }

    // Note: on BSDs, you can just setsockopt(SO_NOSIGPIPE) :^)
    int enable = 1;
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        perror("setsockopt(SO_REUSEADDR)");
        return -1;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    socklen_t addrlen = sizeof(server_addr);

    if (bind(listen_sock, reinterpret_cast<sockaddr*>(&server_addr), addrlen) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(listen_sock, MAX_QUEUED_CONNECTIONS) < 0) {
        perror("listen");
        return -1;
    }

    return listen_sock;
}

void ServerBase::serve()
{
    m_epoll_fd = epoll_create1(0);
    if (m_epoll_fd < 0) {
        perror("epoll_create1");
        exit(1);
    }

    Event event;
    char buf[BUF_SIZE];

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
                if (!handle_new_connection(event))
                    exit(1);
            } else {
                if (!handle_existing_connection(event_fd, buf))
                    exit(1);
            }
        }
    }
}

bool ServerBase::handle_new_connection(Event& event)
{
    static struct sockaddr_in client_addr;
    static socklen_t addrlen = sizeof(client_addr);

    int conn_sock = accept(m_listen_sock, reinterpret_cast<sockaddr*>(&client_addr), &addrlen);
    if (conn_sock == -1) {
        perror("accept");
        return false;
    }
    event.events = EPOLLIN;
    event.data.fd = conn_sock;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, conn_sock, &event) == -1) {
        perror("epoll_ctl");
        return false;
    }
    auto client_ip_addr = client_addr.sin_addr;
    char client_ip[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &client_ip_addr, client_ip, INET_ADDRSTRLEN) == nullptr) {
        perror("inet_ntop");
        return false;
    }
    std::cout << "Got a new connection from " << client_ip << " on fd " << conn_sock << std::endl;
    return on_connect(conn_sock, client_ip);
}

bool ServerBase::handle_existing_connection(int fd, char* buf)
{
    std::memset(buf, 0, BUF_SIZE);
    int n_bytes = read(fd, buf, BUF_SIZE - 1);
    if (n_bytes < 0) {
        perror("read");
        return false;
    }

    std::cout << "req(" << std::dec << n_bytes << "):";
    for (int i = 0; i < n_bytes; ++i) {
        std::cout << " " << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(buf[i]);
    }
    std::cout << " [" << buf << "]" << std::endl;

    if (n_bytes == 0) {
        std::cout << "closing fd " << fd << std::endl;
        ::close(fd);
        return true;
    }

    return on_read(fd, buf, n_bytes);
}

ServerBase::~ServerBase()
{
    close();
}

void ServerBase::close()
{
    ::close(m_epoll_fd);
    ::close(m_listen_sock);
}
