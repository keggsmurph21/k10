#include "Server/Client.h"

namespace k10engine::Server {

static u16 s_next_client_id = 1;

Client::Client(int fd, std::string ip)
    : m_fd(fd)
    , m_ip(std::move(ip))
    , m_id(s_next_client_id++)
{
}

Client::~Client()
{
    for (auto* pending_response : m_pending_responses)
        delete pending_response;
}

void Client::queue_response(const Response* response)
{
    m_pending_responses.push_back(response);
}

bool Client::for_each_pending_response(const std::function<bool(const Response*)>& callback)
{
    bool ok = true;
    for (auto* pending_response : m_pending_responses) {
        ok = ok && callback(pending_response);
        delete pending_response;
    }
    m_pending_responses.clear();
    return ok;
}

bool Client::is_interested_in(Game::GameId game_id) const
{
    // FIXME: Obviously inefficient :^)
    for (const auto& interesting_game_id : m_interesting_game_ids) {
        if (game_id == interesting_game_id)
            return true;
    }
    return false;
}

void Client::set_interested_in(Game::GameId game_id)
{
    m_interesting_game_ids.push_back(game_id);
}

} // namespace k10engine::Server
