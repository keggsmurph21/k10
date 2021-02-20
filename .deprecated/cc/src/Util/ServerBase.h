#pragma once

#include <sys/epoll.h>

#include "Util/ByteBuffer.h"

#define BUF_SIZE 4096
#define MAX_QUEUED_CONNECTIONS 101
#define MAX_QUEUED_EPOLL_EVENTS 100

class ServerBase {
public:
    template<typename T, typename... Args>
    static inline T* create(int port, Args&&... args)
    {
        auto listen_sock = bind_and_listen(port);
        if (listen_sock < 0)
            return nullptr;
        return new T(port, listen_sock, std::forward<Args>(args)...);
    }

    [[noreturn]] void serve();
    void close();

protected:
    using Event = struct epoll_event;

    [[nodiscard]] bool handle_new_connection(Event&);
    [[nodiscard]] virtual bool on_connect(int fd, std::string client_ip) = 0;

    [[nodiscard]] bool handle_ready_to_read(int fd);
    [[nodiscard]] virtual bool on_read(int fd, ByteBuffer&) = 0;

    [[nodiscard]] bool handle_ready_to_write(int fd);
    [[nodiscard]] virtual bool on_writeable(int fd) = 0;
    [[nodiscard]] bool send(int fd, ByteBuffer&);

    [[nodiscard]] virtual bool on_disconnect(int fd) = 0;

    [[noreturn]] void shutdown(int exit_code);
    virtual int on_shutdown(int exit_code) { return exit_code; }

    static int bind_and_listen(int port);

    ServerBase(int port, int listen_sock);
    virtual ~ServerBase();

private:
    int m_port;
    int m_listen_sock;
    int m_epoll_fd;
    Event m_events[MAX_QUEUED_EPOLL_EVENTS];
};
