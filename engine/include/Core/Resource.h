#pragma once

#include <iostream>
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

std::ostream& operator<<(std::ostream&, const NonYieldingResource&);
std::ostream& operator<<(std::ostream&, const Resource&);
std::ostream& operator<<(std::ostream&, const ResourceCounts&);
std::ostream& operator<<(std::ostream&, const ResourceCollection&);
std::ostream& operator<<(std::ostream&, const AbstractResource&);

} // namespace k10engine
