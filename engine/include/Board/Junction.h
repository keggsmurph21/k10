#pragma once

#include <optional>

#include "Board/Node.h"
#include "Board/Orientation.h"

namespace k10engine {

namespace Board {

class Junction : public Node {
public:
    Junction(int index)
        : m_index(index)
    {
        (void)m_index;
    }

    std::optional<Orientation> port_orientation();
    std::optional<Junction*> port_partner();
    void set_port(Orientation, Junction*);

    bool is_port();

private:
    int m_index;
    std::optional<Orientation> m_port_orientation;
    std::optional<Junction*> m_port_partner;
};

} // namespace Board

} // namespace k10engine
