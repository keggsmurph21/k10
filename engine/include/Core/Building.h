#pragma once

#include <iostream>

namespace k10engine {

enum class Building {
    City,
    DevelopmentCard,
    Road,
    Settlement,
};

static const Building AllBuildings[] = {
    Building::City,
    Building::DevelopmentCard,
    Building::Road,
    Building::Settlement,
};

std::ostream& operator<<(std::ostream&, Building);

} // namespace k10engine

