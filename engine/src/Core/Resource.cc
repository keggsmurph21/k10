#include <algorithm>
#include <unordered_set>

#include "Core/Random.h"
#include "Core/Resource.h"

namespace k10engine {

std::ostream& operator<<(std::ostream& os, const NonYieldingResource& type)
{
    switch (type) {
    case NonYieldingResource::Desert:
        os << "NonYieldingResource::Desert";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Resource& type)
{
    switch (type) {
    case Resource::Brick:
        os << "Resource::Brick";
        break;
    case Resource::Ore:
        os << "Resource::Ore";
        break;
    case Resource::Sheep:
        os << "Resource::Sheep";
        break;
    case Resource::Wheat:
        os << "Resource::Wheat";
        break;
    case Resource::Wood:
        os << "Resource::Wood";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const ResourceCounts& counts)
{
    bool is_first_iter = true;
    os << "ResourceCounts{ ";
    for (const auto& it : counts) {
        if (!is_first_iter) {
            os << ", ";
        }
        os << it.first << ": " << it.second;
        is_first_iter = false;
    }
    os << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const ResourceCollection& resources)
{
    bool is_first_iter = true;
    os << "ResourceCollection{ ";
    for (const auto& resource : resources) {
        if (!is_first_iter) {
            os << ", ";
        }
        os << resource;
        is_first_iter = false;
    }
    os << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const AbstractResource& abstract_resource)
{
    if (std::holds_alternative<Resource>(abstract_resource)) {
        os << std::get<Resource>(abstract_resource);
    } else {
        os << std::get<NonYieldingResource>(abstract_resource);
    }
    return os;
}

bool operator<(const ResourceCounts& l_resources, const ResourceCounts& r_resources)
{
    for (const auto& r_it : r_resources) {
        const auto& r_resource = r_it.first;
        const auto& r_count = r_it.second;
        if (r_count == 0) {
            continue;
        }
        if (l_resources.find(r_resource) == l_resources.end()) {
            return true;
        }
        if (l_resources.at(r_resource) < r_count) {
            return true;
        }
    }
    return false;
}

bool operator==(const ResourceCounts& l_resources, const ResourceCounts& r_resources)
{
    std::unordered_set<Resource> seen;
    for (const auto& r_it : r_resources) {
        const auto& r_resource = r_it.first;
        const auto& r_count = r_it.second;
        if (r_count == 0) {
            continue;
        }
        seen.insert(r_resource);
        if (l_resources.find(r_resource) == l_resources.end()) {
            return false;
        }
        if (l_resources.at(r_resource) != r_count) {
            return false;
        }
    }
    for (const auto& l_it : l_resources) {
        const auto& l_resource = l_it.first;
        const auto& l_count = l_it.second;
        if (l_count == 0) {
            continue;
        }
        if (seen.find(l_resource) == seen.end()) {
            return false;
        }
    }
    return true;
}

Resource choose_from(const ResourceCounts& resources)
{
    std::vector<Resource> choices;
    for (const auto& it : resources) {
        const auto resource = it.first;
        const auto& count = it.second;
        for (size_t i = 0; i < count; ++i) {
            choices.push_back(resource);
        }
    }
    std::shuffle(choices.begin(), choices.end(), Random::rng());
    return choices.at(0);
}

} // namespace k10engine
