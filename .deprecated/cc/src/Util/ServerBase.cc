#include <arpa/inet.h>
#include <bitset>
#include <cassert>
#include <csignal>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <unistd.h>

#include "Util/ServerBase.h"

//#define TRACE_SERVER_BASE

ServerBase::ServerBase(int port, int listen_sock)
    : m_port(port)
    , m_listen_sock(listen_sock)
{
    struct sigaction ignore_sigpipe;
    ignore_sigpipe.sa_handler = SIG_IGN;
    sigemptyset(&ignore_sigpipe.sa_mask);
    sigaction(SIGPIPE, &ignore_sigpipe, nullptr);
    std::cerr << "ServerBase: Listening on port " << port << std::endl;
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
        shutdown(1);
    }

    Event event;

    event.events = EPOLLIN;
    event.data.fd = m_listen_sock;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_listen_sock, &event) != 0) {
        perror("epoll_ctl");
        shutdown(1);
    }

#ifdef TRACE_SERVER_BASE
    std::cerr << "ServerBase::serve(): got epoll fd: " << m_listen_sock << std::endl;
#endif

    for (;;) {
        int num_fds = epoll_wait(m_epoll_fd, m_events, MAX_QUEUED_EPOLL_EVENTS, -1); // no timeout
        if (num_fds == -1) {
            perror("epoll_wait");
            shutdown(1);
        }

#ifdef TRACE_SERVER_BASE
        std::cerr << "ServerBase::serve(): Woke up for " << num_fds << " events" << std::endl;
#endif
        for (int i = 0; i < num_fds; ++i) {
            auto event = m_events[i];
            auto event_fd = event.data.fd;
#ifdef TRACE_SERVER_BASE
            std::cerr << "ServerBase::serve(): Handling event " << std::dec << i + 1 << " of " << num_fds
                      << ": fd=" << event_fd << ", mask=" << std::bitset<32>(event.events) << std::endl;
#endif
            if (event_fd == m_listen_sock) {
                if (!handle_new_connection(event))
                    shutdown(0);
            } else if (event.events & EPOLLIN) {
                if (!handle_ready_to_read(event_fd))
                    shutdown(0);
            } else if (event.events & EPOLLOUT) {
                if (!handle_ready_to_write(event_fd))
                    shutdown(0);
            } else {
#ifdef TRACE_SERVER_BASE
                std::cerr << "ServerBase::serve(): Unknown epoll event, exiting ..." << std::endl;
#endif
                shutdown(1);
            }
        }
    }
}

void ServerBase::shutdown(int exit_code)
{
    exit_code = on_shutdown(exit_code);
#ifdef TRACE_SERVER_BASE
    std::cerr << "ServerBase::shutdown(" << exit_code << ")" << std::endl;
#endif
    close();
    exit(exit_code);
}

bool ServerBase::handle_new_connection(Event& event)
{
    static struct sockaddr_in client_addr;
    static socklen_t addrlen = sizeof(client_addr);

    int conn_sock =
        accept4(m_listen_sock, reinterpret_cast<sockaddr*>(&client_addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (conn_sock == -1) {
        perror("accept");
        return false;
    }

    event.events = EPOLLIN | EPOLLOUT;
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
#ifdef TRACE_SERVER_BASE
    std::cerr << "ServerBase::handle_new_connection(): Got a new connection from " << client_ip
              << " on fd=" << conn_sock << std::endl;
#endif
    return on_connect(conn_sock, client_ip);
}

bool ServerBase::handle_ready_to_read(int fd)
{
    static char read_buf[BUF_SIZE];
    ByteBuffer buf;

    for (;;) {
        std::memset(read_buf, 0, BUF_SIZE - 1);

        int n_read = read(fd, read_buf, BUF_SIZE);
        if (n_read < 0) {
            if (errno & EAGAIN || errno & EWOULDBLOCK) {
#ifdef TRACE_SERVER_BASE
                std::cerr << "ServerBase::handle_ready_to_read(" << fd << "): Received " << buf.unread_size()
                          << " bytes: " << buf << std::endl;
#endif
                return on_read(fd, buf);
            }
            perror("read");
            return false;
        }

        if (n_read == 0) {
#ifdef TRACE_SERVER_BASE
            std::cerr << "ServerBase::handle_ready_to_read(" << fd << "): Received 0 bytes, disconnecting client"
                      << std::endl;
#endif
            auto ret = on_disconnect(fd);
            ::close(fd);
            return ret;
        }

        buf.concat(read_buf, n_read);
    }
}

bool ServerBase::send(int fd, ByteBuffer& buf)
{
#ifdef TRACE_SERVER_BASE
    std::cerr << "ServerBase::send(" << fd << "): Writing " << buf.unread_size() << " bytes: " << buf << std::endl;
#endif
    for (;;) {
        int n_written = ::write(fd, buf.data() + buf.cursor(), buf.unread_size());
        buf.seek(buf.cursor() + n_written);

        if (n_written < 0) {
            if (errno & EAGAIN || errno & EWOULDBLOCK) {
                return true;
            }
            if (errno & EPIPE) {
                std::cerr << "ServerBase::send(" << fd << "): Broken pipe! Ignoring ..." << std::endl;
                return true;
            }
            perror("write");
            return false;
        }

        if (n_written == 0) {
            assert(false);
#ifdef TRACE_SERVER_BASE
            std::cerr << "ServerBase::send(" << fd << "): Wrote 0 bytes, disconnecting client" << std::endl;
#endif
            ::close(fd);
            return true;
        }

        if (buf.unread_size() == 0)
            return true;
    }
}

bool ServerBase::handle_ready_to_write(int fd)
{
#ifdef TRACE_SERVER_BASE
    std::cerr << "ServerBase::handle_ready_to_write(" << fd << ")" << std::endl;
#endif
    return on_writeable(fd);
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
