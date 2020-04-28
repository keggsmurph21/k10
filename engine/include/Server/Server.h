#include <functional>
#include <signal.h>
#include <sys/epoll.h>

#define MAX_QUEUED_CONNECTIONS 100
#define MAX_QUEUED_EPOLL_EVENTS 100

namespace k10engine::Server {

class Server {
public:
    static Server* create(int port);
    virtual ~Server();

    [[noreturn]] void serve();
    void close();

    void on_connect(int fd, char* client_ip);
    void on_read(int fd, char* buf, int len);

private:
    Server(int port, int listen_sock);

    int m_port;
    int m_listen_sock;
    int m_epoll_fd;
    struct epoll_event m_events[MAX_QUEUED_EPOLL_EVENTS];
};

} // namespace k10engine::Server
