#pragma once

#include <iostream>

namespace k10engine {

enum class Building {
    City,
    DevelopmentCard,
    Road,
    Settlement,
};

std::ostream& operator<<(std::ostream&, Building);

} // namespace k10engine

