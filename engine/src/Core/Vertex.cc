#include "Core/Vertex.h"

namespace k10engine::State {

std::ostream& operator<<(std::ostream& os, Vertex vertex)
{
    switch (vertex) {
    case Vertex::AfterDiscarding:
        os << "Vertex::AfterDiscarding";
        break;
    case Vertex::AfterMovingRobber:
        os << "Vertex::AfterMovingRobber";
        break;
    case Vertex::AfterRoll:
        os << "Vertex::AfterRoll";
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
    case Vertex::WaitingForTradeResponses:
        os << "Vertex::WaitingForTradeResponses";
        break;
    }
    return os;
}

} // namespace k10engine::State
