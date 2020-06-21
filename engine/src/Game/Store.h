#pragma once

#include <iostream>
#include <string.h>

#include "Game/Game.h"
#include "Util/LRUCache.h"

namespace k10engine::Game {

// FIXME: This can be shared
using GameId = u64;

class Store {
public:
    Store(std::string path, size_t cache_size);

    void create();
    Game* get(GameId);

    struct Path {
        const std::string* base_dir;
        std::string high_byte;
        std::string next_byte;
        std::string low_bytes;
        Path(const std::string* base_dir, const GameId&);
        [[nodiscard]] std::string high_dir() const;
        [[nodiscard]] std::string next_dir() const;
        [[nodiscard]] std::string full_path() const;
        bool exists_on_disk() const;
    };
    Path path_for(const GameId&) const;

private:
    Game* retrieve_from_disk(const GameId&) const;
    void write_to_disk(const GameId&, const Game&) const;

    std::string m_path;
    LRUCache<GameId, Game> m_cache;
};

} // namespace k10engine::Game
