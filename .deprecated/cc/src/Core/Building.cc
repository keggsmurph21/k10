#include "Core/Building.h"

namespace k10engine {

std::ostream& operator<<(std::ostream& os, Building type)
{
    switch (type) {
    case Building::City:
        os << "Building::City";
        break;
    case Building::DevelopmentCard:
        os << "Building::DevelopmentCard";
        break;
    case Building::Road:
        os << "Building::Road";
        break;
    case Building::Settlement:
        os << "Building::Settlement";
        break;
    }
    return os;
}

} // namespace k10engine

using Building = k10engine::Building;

template<>
void encode(ByteBuffer& buf, const Building& building)
{
    Encoder encoder(buf);
    encoder << static_cast<u8>(building);
}

template<>
bool decode(ByteBuffer& buf, Building& building)
{
    Decoder decoder(buf);
    u8 byte;
    if (!decoder.decode(byte))
        return false;
    building = static_cast<Building>(byte);
    return true;
}
