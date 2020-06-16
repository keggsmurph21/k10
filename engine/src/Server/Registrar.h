#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Util/Decoder.h"
#include "Util/Encoder.h"
#include "Util/LinkedTrie.h"
#include "Util/Types.h"

namespace k10engine::Server {

class Registrar {
public:
    using PlayerId = size_t;
    using PlayerSecret = size_t;

    struct Registration {
        PlayerId player_id;
        PlayerSecret internal_secret;
    };

    [[nodiscard]] std::optional<Registration> register_user(const std::string& name,
                                                            const std::string& external_secret);

    [[nodiscard]] bool validate_player(const PlayerId player_id, const PlayerSecret internal_secret) const;

private:
    struct Record {
        size_t external_secret_index;
        u8 external_secret_len;
        size_t internal_secret_index;
    };

    LinkedTrie<const char, Record> m_records;
    std::vector<char> m_external_secrets;
    std::vector<PlayerSecret> m_internal_secrets;
};

} // namespace k10engine::Server

template<>
void encode(ByteBuffer&, k10engine::Server::Registrar::Registration&);

template<>
void encode(ByteBuffer&, const k10engine::Server::Registrar::Registration&);

template<>
bool decode(ByteBuffer&, k10engine::Server::Registrar::Registration&);
