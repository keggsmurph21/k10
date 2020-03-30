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
