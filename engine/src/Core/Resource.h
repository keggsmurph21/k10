#pragma once

#include <iostream>
#include <map>
#include <set>
#include <variant>
#include <vector>

#include "Util/Decoder.h"
#include "Util/Encoder.h"
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

typedef std::map<Resource, size_t> ResourceCounts;
typedef std::set<Resource> ResourceCollection;
typedef std::variant<Resource, NonYieldingResource> AbstractResource;

std::ostream& operator<<(std::ostream&, const NonYieldingResource&);
std::ostream& operator<<(std::ostream&, const Resource&);
std::ostream& operator<<(std::ostream&, const ResourceCounts&);
std::ostream& operator<<(std::ostream&, const ResourceCollection&);
std::ostream& operator<<(std::ostream&, const AbstractResource&);

bool operator<(const ResourceCounts& l_resources, const ResourceCounts& r_resources);
bool operator==(const ResourceCounts& l_resources, const ResourceCounts& r_resources);

Resource choose_from(const ResourceCounts&);

} // namespace k10engine

template<>
void encode(ByteBuffer&, const k10engine::AbstractResource&);

template<>
bool decode(ByteBuffer&, k10engine::AbstractResource&);

template<>
void encode(ByteBuffer&, const k10engine::NonYieldingResource&);

template<>
bool decode(ByteBuffer&, k10engine::NonYieldingResource&);

template<>
void encode(ByteBuffer&, const k10engine::Resource&);

template<>
bool decode(ByteBuffer&, k10engine::Resource&);

template<>
void encode(ByteBuffer&, const k10engine::ResourceCollection&);

template<>
bool decode(ByteBuffer&, k10engine::ResourceCollection&);
