#pragma once

#include <iostream>

namespace k10engine {

namespace Board {

enum class NodeType {
    Hex,
    Ocean,
    Junction,
    Road,
    UnflippedHex,
};

std::ostream& operator<<(std::ostream&, NodeType);

} // namespace Board

} // namespace k10engine
