#include "Server/Registrar.h"
#include "Core/Random.h"

namespace k10engine::Server {

bool Registrar::validate_player(const Registrar::PlayerId player_id,
                                const Registrar::PlayerSecret internal_secret) const
{
    if (player_id >= m_internal_secrets.size())
        return false;
    return m_internal_secrets.at(player_id) == internal_secret;
}

static Registrar::PlayerSecret get_internal_secret()
{
    // random number in [0, 0xffffffffffffffff]
    static auto s_random_64bit_dist =
        std::uniform_int_distribution<Registrar::PlayerSecret>(0, std::numeric_limits<Registrar::PlayerSecret>::max());
    static auto rng = Random::rng();
    return s_random_64bit_dist(rng);
}

std::optional<Registrar::Registration> Registrar::register_user(const std::string& name,
                                                                const std::string& external_secret)
{
    if (name.size() == 0)
        return {};
    if (external_secret.size() == 0)
        return {};
    size_t internal_secret_index;
    if (m_records.contains(name.c_str(), name.size())) {
        auto* record = m_records.get(name.c_str(), name.size());
        const auto offset = record->external_secret_index;
        if (external_secret.size() != record->external_secret_len)
            return {};
        for (auto i = 0; i < record->external_secret_len; ++i) {
            if (m_external_secrets.at(offset + i) != external_secret.at(i))
                return {};
        }
        internal_secret_index = record->internal_secret_index;
    } else {
        Record record = {};
        record.external_secret_index = m_external_secrets.size();
        for (auto ch : external_secret)
            m_external_secrets.push_back(ch);
        record.external_secret_len = external_secret.size();
        record.internal_secret_index = m_internal_secrets.size();
        m_internal_secrets.push_back(get_internal_secret());
        internal_secret_index = record.internal_secret_index;
        m_records.set(name.c_str(), name.size(), record);
    }
    Registration registration{ internal_secret_index, m_internal_secrets.at(internal_secret_index) };
    return registration;
}

} // namespace k10engine::Server

template<>
void encode(ByteBuffer& buf, k10engine::Server::Registrar::Registration& registration)
{
    Encoder encoder(buf);
    encoder << registration.player_id;
    encoder << registration.internal_secret;
}

template<>
void encode(ByteBuffer& buf, const k10engine::Server::Registrar::Registration& registration)
{
    Encoder encoder(buf);
    encoder << registration.player_id;
    encoder << registration.internal_secret;
}

template<>
bool decode(ByteBuffer& buf, k10engine::Server::Registrar::Registration& registration)
{
    Decoder decoder(buf);
    k10engine::Server::Registrar::PlayerId player_id;
    if (!decoder.decode(player_id))
        return false;
    k10engine::Server::Registrar::PlayerSecret player_secret;
    if (!decoder.decode(player_secret))
        return false;
    registration = { player_id, player_secret };
    return true;
}
