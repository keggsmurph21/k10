#pragma once

#include <vector>

#include "Util/Types.h"

namespace k10engine {

class Serializable {
public:
    virtual std::vector<u8> serialize() const = 0;
    virtual ~Serializable() {}
};

} // namespace k10engine
