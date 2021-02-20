#pragma once

#include "Game/Game.h"
#include "Game/Server/Persistence.h"
#include "Util/DoublyLinkedList.h"

namespace k10engine::Server {

class GameCache {
public:
    GameCache(GameDatabase& db, size_t max_size);
    Game* get(size_t game_id);

private:
    DoublyLinkedList<Game*> m_storage;
    Game* load(size_t game_id);
};

} // namespace k10engine::Server
