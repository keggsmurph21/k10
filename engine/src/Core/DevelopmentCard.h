#pragma once

#include <iostream>

#include "Util/Decoder.h"
#include "Util/Encoder.h"

namespace k10engine {

enum class DevelopmentCard {
    Knight,
    Monopoly,
    RoadBuilding,
    VictoryPoint,
    YearOfPlenty,
};

static const DevelopmentCard AllDevelopmentCards[] = {
    DevelopmentCard::Knight,       DevelopmentCard::Monopoly,     DevelopmentCard::RoadBuilding,
    DevelopmentCard::VictoryPoint, DevelopmentCard::YearOfPlenty,
};

std::ostream& operator<<(std::ostream&, DevelopmentCard);

} // namespace k10engine

template<>
void encode(ByteBuffer&, const k10engine::DevelopmentCard&);

template<>
bool decode(ByteBuffer&, k10engine::DevelopmentCard&);
