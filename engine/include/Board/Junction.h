#pragma once

#include "Board/Node.h"

namespace k10engine {

namespace Board {

class Junction : public Node {
public:
    Junction(int index)
        : m_index(index)
    {
        (void)m_index;
    }

private:
    int m_index;
};

} // namespace Board

} // namespace k10engine
