#pragma once

#include <map>
#include <set>
#include <variant>

namespace k10engine {

enum class NonYieldingResource {
    Desert,
};

enum class Resource {
    Brick,
    Ore,
    Sheep,
    Wheat,
    Wood,
};

typedef std::map<Resource, int, std::less<>> ResourceCounts;
typedef std::set<Resource, std::less<>> ResourceCollection;
typedef std::variant<Resource, NonYieldingResource> AbstractResource;

} // namespace k10engine
