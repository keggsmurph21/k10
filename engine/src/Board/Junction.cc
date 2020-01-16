#include <assert.h>

#include "Board/Junction.h"

namespace k10engine {

namespace Board {

std::optional<Orientation> Junction::port_orientation()
{
    return m_port_orientation;
}

std::optional<Junction*> Junction::port_partner()
{
    return m_port_partner;
}

void Junction::set_port(Orientation orientation, Junction* port_partner)
{
    assert(port_partner);
    m_port_orientation = orientation;
    m_port_partner = port_partner;
}

bool Junction::is_port()
{
    return m_port_orientation.has_value() && m_port_partner.has_value();
}

} // namespace Board

} // namespace k10engine
