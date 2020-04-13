#pragma once

#include <iostream>
#include <map>
#include <set>
#include <variant>
#include <vector>

#include "Util/Types.h"

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

typedef std::map<Resource, size_t, std::less<>> ResourceCounts;
typedef std::set<Resource, std::less<>> ResourceCollection;
typedef std::variant<Resource, NonYieldingResource> AbstractResource;

std::ostream& operator<<(std::ostream&, const NonYieldingResource&);
std::ostream& operator<<(std::ostream&, const Resource&);
std::ostream& operator<<(std::ostream&, const ResourceCounts&);
std::ostream& operator<<(std::ostream&, const ResourceCollection&);
std::ostream& operator<<(std::ostream&, const AbstractResource&);

bool operator<(const ResourceCounts& l_resources, const ResourceCounts& r_resources);
bool operator==(const ResourceCounts& l_resources, const ResourceCounts& r_resources);

Resource choose_from(const ResourceCounts&);

static u8 magic_non_yielding_resource_byte = 0xf0;

inline u8 serialize(const AbstractResource& resource)
{
    if (std::holds_alternative<Resource>(resource)) {
        return static_cast<u8>(std::get<Resource>(resource));
    } else {
        return static_cast<u8>(std::get<NonYieldingResource>(resource))
               + magic_non_yielding_resource_byte;
    }
}

inline AbstractResource deserialize_abstract_resource(u8 byte)
{
    if ((byte & magic_non_yielding_resource_byte) != 0) {
        return static_cast<NonYieldingResource>(byte - magic_non_yielding_resource_byte);
    } else {
        return static_cast<Resource>(byte);
    }
}

inline std::vector<u8> serialize(const ResourceCollection& resources)
{
    std::vector<u8> serial;
    serial.push_back(static_cast<u8>(resources.size()));
    for (const auto& resource : resources) {
        serial.push_back(serialize(resource));
    }
    return serial;
}

inline ResourceCollection deserialize_resource_collection(const std::vector<u8>& serial,
                                                          size_t& index)
{
    auto size = static_cast<size_t>(serial[index++]);
    ResourceCollection resources;
    for (size_t i = 0; i < size; ++i) {
        resources.insert(static_cast<Resource>(serial[index++]));
    }
    return resources;
}

} // namespace k10engine
