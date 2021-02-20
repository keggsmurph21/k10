#include "Core/DevelopmentCard.h"

namespace k10engine {

std::ostream& operator<<(std::ostream& os, DevelopmentCard type)
{
    switch (type) {
    case DevelopmentCard::Knight:
        os << "DevelopmentCard::Knight";
        break;
    case DevelopmentCard::Monopoly:
        os << "DevelopmentCard::Monopoly";
        break;
    case DevelopmentCard::RoadBuilding:
        os << "DevelopmentCard::RoadBuilding";
        break;
    case DevelopmentCard::VictoryPoint:
        os << "DevelopmentCard::VictoryPoint";
        break;
    case DevelopmentCard::YearOfPlenty:
        os << "DevelopmentCard::YearOfPlenty";
        break;
    }
    return os;
}

} // namespace k10engine

using DevelopmentCard = k10engine::DevelopmentCard;

template<>
void encode(ByteBuffer& buf, const DevelopmentCard& development_card)
{
    Encoder encoder(buf);
    encoder << static_cast<u8>(development_card);
}

template<>
bool decode(ByteBuffer& buf, DevelopmentCard& development_card)
{
    Decoder decoder(buf);
    u8 byte;
    if (!decoder.decode(byte))
        return false;
    development_card = static_cast<DevelopmentCard>(byte);
    return true;
}
