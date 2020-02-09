#pragma once

#include <iostream>

namespace k10engine {

enum class DevelopmentCard {
    Knight,
    Monopoly,
    RoadBuilding,
    VictoryPoint,
    YearOfPlenty,
};

std::ostream& operator<<(std::ostream&, DevelopmentCard);

} // namespace k10engine
