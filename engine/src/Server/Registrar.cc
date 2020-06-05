#include "Server/Registrar.h"
#include "Core/Random.h"

namespace k10engine {

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

std::optional<Registrar::Registration>
Registrar::register_user(const char* name, const u8 name_len, const char* external_secret, const u8 external_secret_len)
{
    if (name == nullptr)
        return {};
    if (name_len == 0)
        return {};
    if (external_secret == nullptr)
        return {};
    if (external_secret_len == 0)
        return {};
    size_t internal_secret_index;
    if (m_records.contains(name, name_len)) {
        auto* record = m_records.get(name, name_len);
        const auto offset = record->external_secret_index;
        if (external_secret_len != record->external_secret_len)
            return {};
        for (auto i = 0; i < record->external_secret_len; ++i) {
            if (m_external_secrets.at(offset + i) != *external_secret++)
                return {};
        }
        internal_secret_index = record->internal_secret_index;
    } else {
        Record record = {};
        record.external_secret_index = m_external_secrets.size();
        for (auto i = 0; i < external_secret_len; ++i) {
            m_external_secrets.push_back(*external_secret++);
        }
        record.external_secret_len = external_secret_len;
        record.internal_secret_index = m_internal_secrets.size();
        m_internal_secrets.push_back(get_internal_secret());
        internal_secret_index = record.internal_secret_index;
        m_records.set(name, name_len, record);
    }
    Registration registration{ internal_secret_index, m_internal_secrets.at(internal_secret_index) };
    return registration;
}

} // namespace k10engine
