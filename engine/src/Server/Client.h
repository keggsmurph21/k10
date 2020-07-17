#pragma once

#include <functional>
#include <vector>

#include "Forward.h"
#include "Server/Response.h"

namespace k10engine::Server {

class Client {
public:
    Client(int fd, std::string ip)
        : m_fd(fd)
        , m_ip(std::move(ip))
    {
    }
    ~Client();

    void queue_response(const Response*);
    [[nodiscard]] bool for_each_pending_response(const std::function<bool(const Response*)>&);

    bool is_interested_in(Game::GameId) const;
    void set_interested_in(Game::GameId);
    // FIXME: Add an API for "losing interest" in a game_id

private:
    int m_fd;
    const std::string m_ip;
    std::vector<const Response*> m_pending_responses;
    std::vector<Game::GameId> m_interesting_game_ids;
};

} // namespace k10engine::Server
