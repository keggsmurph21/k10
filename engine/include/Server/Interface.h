#pragma once

#define FIXME (void*)

namespace k10engine::Server {

class Client;

class Request {
    enum class Type {
        NewGame,
        JoinGame,
        LeaveGame,
        StartGame,
        MakeMove,
        Query,
        RegisterClient,
        UnregisterClient,
    };

protected:
    Request(Client* client, Type type)
        : m_client(client)
        , m_type(type)
    {
    }

private:
    Client* m_client;
    Type m_type;
};

class NewGameRequest final : public Request {
private:
    FIXME m_parameters;
};

class JoinGameRequest final : public Request {
private:
    size_t m_game_id;
};

class LeaveGameRequest final : public Request {
private:
    size_t m_game_id;
};

class StartGameRequest final : public Request {
private:
    size_t m_game_id;
};

class MakeMoveRequest final : public Request {
private:
    size_t m_game_id;
};

class QueryRequest final : public Request {
private:
    size_t m_game_id;
};

class RegisterClientRequest final : public Request {
private:
    size_t m_game_id;
};

class UnregisterClientRequest final : public Request {
private:
    size_t m_game_id;
};

class Response {
    enum class Type {
        NewGame,
        JoinGame,
        LeaveGame,
        StartGame,
        MakeMove,
        Query,
        RegisterClient,
        UnregisterClient,
        GameChanged,
    };

protected:
    Response(Type type)
        : m_type(type)
    {
    }

private:
    Type m_type;
}

} // namespace k10engine::Server
