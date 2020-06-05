#pragma once

#include <optional>
#include <vector>

#include "Util/LinkedTrie.h"
#include "Util/Types.h"

namespace k10engine {

class Registrar {
public:
    using PlayerId = size_t;
    using PlayerSecret = size_t;

    struct Registration {
        PlayerId player_id;
        PlayerSecret internal_secret;
    };

    struct Record {
        size_t external_secret_index;
        u8 external_secret_len;
        size_t internal_secret_index;
    };

    [[nodiscard]] std::optional<Registration>
    register_user(const char* name, const u8 name_len, const char* external_secret, const u8 external_secret_len);

    [[nodiscard]] bool validate_player(const PlayerId player_id, const PlayerSecret internal_secret) const;

private:
    LinkedTrie<const char, Record> m_records;
    std::vector<char> m_external_secrets;
    std::vector<PlayerSecret> m_internal_secrets;
};

} // namespace k10engine
