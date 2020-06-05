#pragma once

#include <optional>
#include <vector>

#include "Util/LinkedTrie.h"
#include "Util/Types.h"

namespace k10engine {

class Registrar {
public:
    struct Registration {
        size_t player_id;
        size_t internal_secret;
    };

    struct Record {
        size_t external_secret_index;
        u8 external_secret_len;
        size_t internal_secret_index;
    };

    [[nodiscard]] std::optional<Registration>
    register_user(const char* name, const u8 name_len, const char* external_secret, const u8 external_secret_len);

    [[nodiscard]] bool validate_player(const size_t player_id, const size_t internal_secret) const;

private:
    LinkedTrie<const char, Record> m_records;
    std::vector<char> m_external_secrets;
    std::vector<size_t> m_internal_secrets;
};

} // namespace k10engine
