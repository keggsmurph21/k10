#include "Core/Edge.h"

namespace k10engine::State {

std::ostream& operator<<(std::ostream& os, Edge edge)
{
    switch (edge) {
    case Edge::AcceptTrade:
        os << "Edge::AcceptTrade";
        break;
    case Edge::BuildCity:
        os << "Edge::BuildCity";
        break;
    case Edge::BuildDevelopmentCard:
        os << "Edge::BuildDevelopmentCard";
        break;
    case Edge::BuildRoad:
        os << "Edge::BuildRoad";
        break;
    case Edge::BuildSettlement:
        os << "Edge::BuildSettlement";
        break;
    case Edge::CancelTrade:
        os << "Edge::CancelTrade";
        break;
    case Edge::ChooseInitialResources:
        os << "Edge::ChooseInitialResources";
        break;
    case Edge::DeclineTrade:
        os << "Edge::DeclineTrade";
        break;
    case Edge::Discard:
        os << "Edge::Discard";
        break;
    case Edge::EndTurn:
        os << "Edge::EndTurn";
        break;
    case Edge::MoveRobber:
        os << "Edge::MoveRobber";
        break;
    case Edge::OfferTrade:
        os << "Edge::OfferTrade";
        break;
    case Edge::PlayKnight:
        os << "Edge::PlayKnight";
        break;
    case Edge::PlayMonopoly:
        os << "Edge::PlayMonopoly";
        break;
    case Edge::PlayRoadBuilding:
        os << "Edge::PlayRoadBuilding";
        break;
    case Edge::PlayVictoryPoint:
        os << "Edge::PlayVictoryPoint";
        break;
    case Edge::PlayYearOfPlenty:
        os << "Edge::PlayYearOfPlenty";
        break;
    case Edge::RollDice:
        os << "Edge::RollDice";
        break;
    case Edge::Steal:
        os << "Edge::Steal";
        break;
    case Edge::ToRoot:
        os << "Edge::ToRoot";
        break;
    case Edge::TradeWithBank:
        os << "Edge::TradeWithBank";
        break;
    }
    return os;
}

} // namespace k10engine::State

using Edge = k10engine::State::Edge;

template<>
void encode(ByteBuffer& buf, const Edge& edge)
{
    Encoder encoder(buf);
    encoder << static_cast<u8>(edge);
}

template<>
bool decode(ByteBuffer& buf, Edge& edge)
{
    Decoder decoder(buf);
    u8 byte;
    if (!decoder.decode(byte))
        return false;

    switch (static_cast<Edge>(byte)) {
    case Edge::AcceptTrade:
        edge = Edge::AcceptTrade;
        break;
    case Edge::BuildCity:
        edge = Edge::BuildCity;
        break;
    case Edge::BuildDevelopmentCard:
        edge = Edge::BuildDevelopmentCard;
        break;
    case Edge::BuildRoad:
        edge = Edge::BuildRoad;
        break;
    case Edge::BuildSettlement:
        edge = Edge::BuildSettlement;
        break;
    case Edge::CancelTrade:
        edge = Edge::CancelTrade;
        break;
    case Edge::ChooseInitialResources:
        edge = Edge::ChooseInitialResources;
        break;
    case Edge::DeclineTrade:
        edge = Edge::DeclineTrade;
        break;
    case Edge::Discard:
        edge = Edge::Discard;
        break;
    case Edge::EndTurn:
        edge = Edge::EndTurn;
        break;
    case Edge::MoveRobber:
        edge = Edge::MoveRobber;
        break;
    case Edge::OfferTrade:
        edge = Edge::OfferTrade;
        break;
    case Edge::PlayKnight:
        edge = Edge::PlayKnight;
        break;
    case Edge::PlayMonopoly:
        edge = Edge::PlayMonopoly;
        break;
    case Edge::PlayRoadBuilding:
        edge = Edge::PlayRoadBuilding;
        break;
    case Edge::PlayVictoryPoint:
        edge = Edge::PlayVictoryPoint;
        break;
    case Edge::PlayYearOfPlenty:
        edge = Edge::PlayYearOfPlenty;
        break;
    case Edge::RollDice:
        edge = Edge::RollDice;
        break;
    case Edge::Steal:
        edge = Edge::Steal;
        break;
    case Edge::ToRoot:
        edge = Edge::ToRoot;
        break;
    case Edge::TradeWithBank:
        edge = Edge::TradeWithBank;
        break;
    default:
        return false;
    }
    return true;
}
