#pragma once

#include "Board/Node.h"

namespace k10engine {

namespace Board {

class Hex : public Node {
public:
    Hex(int index)
        : m_index(index)
    {
        (void)m_index;
    }

private:
    int m_index;
};

} // namespace Board

} // namespace k10engine
