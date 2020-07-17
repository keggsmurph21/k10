#pragma once

#include <vector>

#include "Scenario/Scenario.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"
#include "Util/LRUCache.h"

namespace k10engine::Scenario {

enum class Name {
    Single,
    Triple,
    Standard,
};

class Store {
public:
    static const Store& the();
    ~Store();

    const Scenario* by_name(const Name&) const;
    Name name_of(const Scenario*) const;

private:
    Store();
    std::vector<const Scenario*> m_named_scenarios;
};

} // namespace k10engine::Scenario

template<>
void encode(ByteBuffer&, const k10engine::Scenario::Name&);

template<>
bool decode(ByteBuffer&, k10engine::Scenario::Name&);
