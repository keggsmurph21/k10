#pragma once

#include "Board/Node.h"

namespace k10engine {

namespace Board {

class Ocean : public Node {
public:
    Ocean(int index)
        : m_index(index)
    {
        (void)m_index;
    }

private:
    int m_index;
};

} // namespace Board

} // namespace k10engine
