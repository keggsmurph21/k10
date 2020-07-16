#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

#include "Board/Store.h"
#include "Server/GameCache.h"

namespace k10engine::Server {

GameCache::GameCache(std::string path, size_t cache_size)
    : m_path(std::move(path))
    , m_cache({ [this](const Game::GameId& game_id) -> Game::Game* { return this->retrieve_from_disk(game_id); },
                [this](const Game::GameId& game_id, Game::Game* game) -> void { this->write_to_disk(game_id, *game); },
                cache_size })
{
}

void GameCache::set(Game::GameId game_id, Game::Game* game)
{
    m_cache.set(game_id, game);
}

Game::Game* GameCache::get(Game::GameId game_id)
{
    return m_cache.get(game_id);
}

static std::string byte_for_path(const Game::GameId& game_id, u8 offset)
{
    u64 mask = 0xff;
    u8 high_byte = (game_id & (mask << offset)) >> offset;
    std::ostringstream path;
    path << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(high_byte);
    return path.str();
}

GameCache::Path::Path(const std::string* base_dir, const Game::GameId& game_id)
    : base_dir(base_dir)
{
    high_byte = byte_for_path(game_id, 56);
    next_byte = byte_for_path(game_id, 48);
    std::ostringstream low_bytes_stream;
    u64 low_bytes_value = game_id & 0x0000ffffffffffff;
    low_bytes_stream << std::hex << std::setfill('0') << std::setw(12) << low_bytes_value;
    low_bytes = low_bytes_stream.str();
}

std::string GameCache::Path::high_dir() const
{
    std::ostringstream path;
    path << *base_dir << "/" << high_byte;
    return path.str();
}

std::string GameCache::Path::next_dir() const
{
    std::ostringstream path;
    path << *base_dir << "/" << high_byte << "/" << next_byte;
    return path.str();
}

[[nodiscard]] std::string GameCache::Path::full_path() const {
    std::ostringstream path;
    path << *base_dir << "/" << high_byte << "/" << next_byte << "/" << low_bytes << ".game";
    return path.str();
}

GameCache::Path GameCache::path_for(const Game::GameId& game_id) const
{
    // If a (u64) game_id is given by { de ad be ef de ad be ef }, then
    // we'll try to store the binary representation of the game at
    //
    //      file://${this->m_path}/de/ad/beefdeadbeef.game
    //
    GameCache::Path path(&m_path, game_id);
    return path;
}

bool GameCache::Path::exists_on_disk() const
{
    if (access(base_dir->c_str(), R_OK | W_OK) != 0)
        return false;
    if (access(high_dir().c_str(), R_OK | W_OK) != 0)
        return false;
    if (access(next_dir().c_str(), R_OK | W_OK) != 0)
        return false;
    return access(full_path().c_str(), R_OK | W_OK) == 0;
}

void GameCache::Path::ensure_directories() const
{
    // If the base dir doesn't exist, we should panic
    if (access(base_dir->c_str(), R_OK | W_OK) != 0) {
        std::cerr << "Path::ensure_directories(): access(" << base_dir << ") failed" << std::endl;
        assert(false);
    }
    if (access(high_dir().c_str(), R_OK | W_OK) != 0) {
        if (mkdir(high_dir().c_str(), 0700) < 0) {
            perror("mkdir");
            assert(false);
        }
    }
    if (access(next_dir().c_str(), R_OK | W_OK) != 0) {
        if (mkdir(next_dir().c_str(), 0700) < 0) {
            perror("mkdir");
            assert(false);
        }
    }
}

Game::Game* GameCache::retrieve_from_disk(const Game::GameId& game_id) const
{
    const auto path = path_for(game_id);
    // std::cout << "Server::GameCache: retrieving game " << game_id << " from file://" << path.full_path() <<
    // std::endl;
    if (!path.exists_on_disk()) {
        // std::cout << "Server::GameCache: couldn't find game " << game_id << "!" << std::endl;
        return nullptr;
    }

    ByteBuffer buf;
    {
        std::ifstream fp(path.full_path(), std::ios::binary);
        if (!fp.is_open()) {
            perror("read");
            return nullptr;
        }
        char tmp_buf[1024]; // read 1kb at a time
        size_t n_read = 0;
        do {
            if (!fp.read(tmp_buf, sizeof(tmp_buf)) && !fp.eof()) {
                perror("read");
                return nullptr;
            }
            n_read = fp.gcount();
            buf.concat(tmp_buf, n_read);
        } while (n_read > 0);
    }
    return Game::Game::decode(buf);
}

void GameCache::write_to_disk(const Game::GameId& game_id, const Game::Game& game) const
{
    const auto path = path_for(game_id);
    // std::cout << "Server::GameCache: writing game " << game_id << " to file://" << path.full_path() << std::endl;
    path.ensure_directories();

    ByteBuffer buf;
    Encoder encoder(buf);
    encoder << game;

    {
        std::ofstream fp{ path.full_path(), std::ios::binary | std::ios::out };
        if (!fp.is_open()) {
            perror("write");
            assert(false);
        }
        fp.write(reinterpret_cast<char*>(buf.data()), buf.unread_size());
    }
}

} // namespace k10engine::Server
