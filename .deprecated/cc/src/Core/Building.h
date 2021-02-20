#pragma once

#include <iostream>

#include "Util/Decoder.h"
#include "Util/Encoder.h"

namespace k10engine {

enum class Building {
    City,
    DevelopmentCard,
    Road,
    Settlement,
};

std::ostream& operator<<(std::ostream&, Building);

} // namespace k10engine

template<>
void encode(ByteBuffer&, const k10engine::Building&);

template<>
bool decode(ByteBuffer&, k10engine::Building&);
