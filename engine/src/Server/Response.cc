#include <cassert>

#include "Server/Response.h"

namespace k10engine::Server {

void Response::encode(ByteBuffer& buf) const
{
    buf.bytes.push_back(static_cast<u8>(m_type));
}

void RegisterUserResponse::encode(ByteBuffer& buf) const
{
    Response::encode(buf);

    if (!m_registration.has_value()) {
        buf.bytes.push_back(0);
        return;
    }

    buf.bytes.push_back(1);

    buf.bytes.push_back(static_cast<u8>((m_registration->player_id >> 56) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->player_id >> 48) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->player_id >> 40) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->player_id >> 32) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->player_id >> 24) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->player_id >> 16) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->player_id >> 8) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->player_id >> 0) & 0xff));

    buf.bytes.push_back(static_cast<u8>((m_registration->internal_secret >> 56) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->internal_secret >> 48) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->internal_secret >> 40) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->internal_secret >> 32) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->internal_secret >> 24) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->internal_secret >> 16) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->internal_secret >> 8) & 0xff));
    buf.bytes.push_back(static_cast<u8>((m_registration->internal_secret >> 0) & 0xff));
}

} // namespace k10engine::Server
