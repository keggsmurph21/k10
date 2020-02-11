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

static const DevelopmentCard AllDevelopmentCards[] = {
    DevelopmentCard::Knight,       DevelopmentCard::Monopoly,     DevelopmentCard::RoadBuilding,
    DevelopmentCard::VictoryPoint, DevelopmentCard::YearOfPlenty,
};

std::ostream& operator<<(std::ostream&, DevelopmentCard);

} // namespace k10engine
