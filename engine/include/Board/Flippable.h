#pragma once

#include "Node.h"

namespace k10engine {

namespace Board {

class Flippable : public Node {
public:
    Flippable(int index)
        : m_index(index)
    {
        (void)m_index;
    }

private:
    int m_index;
};

} // namespace Board

} // namespace k10engine
