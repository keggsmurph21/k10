#pragma once

#include <iostream>
#include <string.h>

#include "Game/Game.h"
#include "Util/LRUCache.h"

namespace k10engine::Server {

class GameCache {
public:
    GameCache(std::string path, size_t cache_size);

    Game::Game* get(Game::GameId);
    void set(Game::GameId, Game::Game*);

    struct Path {
        const std::string* base_dir;
        std::string high_byte;
        std::string next_byte;
        std::string low_bytes;
        Path(const std::string* base_dir, const Game::GameId&);
        [[nodiscard]] std::string high_dir() const;
        [[nodiscard]] std::string next_dir() const;
        [[nodiscard]] std::string full_path() const;
        bool exists_on_disk() const;
        void ensure_directories() const;
    };
    Path path_for(const Game::GameId&) const;

private:
    Game::Game* retrieve_from_disk(const Game::GameId&) const;
    void write_to_disk(const Game::GameId&, const Game::Game&) const;

    std::string m_path;
    LRUCache<Game::GameId, Game::Game> m_cache;
};

} // namespace k10engine::Server
