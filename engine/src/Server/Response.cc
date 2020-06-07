#include <cassert>

#include "Server/Response.h"

namespace k10engine::Server {

void Response::encode(Encoder& encoder) const
{
    encoder << m_type;
}

void RegisterUserResponse::encode(Encoder& encoder) const
{
    Response::encode(encoder);
    encoder << m_registration;
}

} // namespace k10engine::Server

template<>
void encode(ByteBuffer& buf, k10engine::Server::Response::Type& type)
{
    buf.append(static_cast<u8>(type));
}

template<>
void encode(ByteBuffer& buf, const k10engine::Server::Response::Type& type)
{
    buf.append(static_cast<u8>(type));
}

template<>
bool decode(ByteBuffer& buf, k10engine::Server::Response::Type& type)
{
    if (buf.unread_size() < 1)
        return false;
    u8 enum_value = buf.read();
    if (enum_value > static_cast<u8>(k10engine::Server::Response::Type::GameChanged))
        return false;
    type = static_cast<k10engine::Server::Response::Type>(enum_value);
    return true;
}
