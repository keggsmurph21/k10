#include "Core/Vertex.h"

namespace k10engine::State {

std::ostream& operator<<(std::ostream& os, Vertex vertex)
{
    switch (vertex) {
    case Vertex::AfterBuildingFreeSettlement:
        os << "Vertex::AfterBuildingFreeSettlement";
        break;
    case Vertex::AfterDiscarding:
        os << "Vertex::AfterDiscarding";
        break;
    case Vertex::AfterMovingRobber:
        os << "Vertex::AfterMovingRobber";
        break;
    case Vertex::AfterRollingSeven:
        os << "Vertex::AfterRollingSeven";
        break;
    case Vertex::ChooseInitialResources:
        os << "Vertex::ChooseInitialResources";
        break;
    case Vertex::GameOver:
        os << "Vertex::GameOver";
        break;
    case Vertex::Root:
        os << "Vertex::Root";
        break;
    case Vertex::WaitForTurn:
        os << "Vertex::WaitForTurn";
        break;
    case Vertex::WaitForTradeResponses:
        os << "Vertex::WaitForTradeResponses";
        break;
    }
    return os;
}

} // namespace k10engine::State

using Vertex = k10engine::State::Vertex;

template<>
void encode(ByteBuffer& buf, const Vertex& vertex)
{
    Encoder encoder(buf);
    encoder << static_cast<u8>(vertex);
}

template<>
bool decode(ByteBuffer& buf, Vertex& vertex)
{
    Decoder decoder(buf);
    u8 byte;
    if (!decoder.decode(byte))
        return false;
    vertex = static_cast<Vertex>(byte);
    return true;
}
