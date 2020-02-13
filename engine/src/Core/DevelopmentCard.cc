#include "Core/DevelopmentCard.h"

namespace k10engine {

std::ostream& operator<<(std::ostream& os, DevelopmentCard type)
{
    switch (type) {
    case DevelopmentCard::Knight:
        os << "DevelopmentCard::Knight";
        break;
    case DevelopmentCard::Monopoly:
        os << "DevelopmentCard::Monopoly";
        break;
    case DevelopmentCard::RoadBuilding:
        os << "DevelopmentCard::RoadBuilding";
        break;
    case DevelopmentCard::VictoryPoint:
        os << "DevelopmentCard::VictoryPoint";
        break;
    case DevelopmentCard::YearOfPlenty:
        os << "DevelopmentCard::YearOfPlenty";
        break;
    }
    return os;
}

} // namespace k10engine
