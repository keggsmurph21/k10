#include <cassert>

#include "Server/Response.h"

namespace k10engine::Server {

void Response::encode(ByteBuffer& buf) const
{
    buf.bytes.push_back(static_cast<u8>(m_type));
}

} // namespace k10engine::Server
