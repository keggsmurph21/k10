#pragma once

#include <map>

namespace k10engine {

namespace Game {

enum class Resource { Brick, Ore, Sheep, Wheat, Wood };

typedef std::map<Resource, int, std::less<>> ResourceCounts;

} // namespace Game

} // namespace k10engine
