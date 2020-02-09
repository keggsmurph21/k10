#pragma once

#include <string.h>

#include "Core/Resource.h"

namespace k10engine {

namespace Game {

class Trade {
public:
    Trade(const std::string& offerer_id,
          const ResourceCounts& cards_from_offerer,
          const ResourceCounts& cards_to_offerer)
        : m_offerer_id(offerer_id)
        , m_cards_from_offerer(cards_from_offerer)
        , m_cards_to_offerer(cards_to_offerer)
    {
    }

private:
    const std::string& m_offerer_id;
    const ResourceCounts& m_cards_from_offerer;
    const ResourceCounts& m_cards_to_offerer;
};

} // namespace Game

} // namespace k10engine
