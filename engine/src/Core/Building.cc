#include "Core/Building.h"

namespace k10engine {

std::ostream& operator<<(std::ostream& os, Building type)
{
    switch (type) {
    case Building::City:
        os << "Building::City";
        break;
    case Building::DevelopmentCard:
        os << "Building::DevelopmentCard";
        break;
    case Building::Road:
        os << "Building::Road";
        break;
    case Building::Settlement:
        os << "Building::Settlement";
        break;
    }
    return os;
}

} // namespace k10engine
