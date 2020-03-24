#include "Core/Edge.h"

namespace k10engine::State {

std::ostream& operator<<(std::ostream& os, Edge edge)
{
    switch (edge) {
    case Edge::AcceptTrade:
        os << "Edge::AcceptTrade";
        break;
    case Edge::Build:
        os << "Edge::Build";
        break;
    case Edge::CancelTrade:
        os << "Edge::CancelTrade";
        break;
    case Edge::ChooseInitialResources:
        os << "Edge::ChooseInitialResources";
        break;
    case Edge::CollectResources:
        os << "Edge::CollectResources";
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
    case Edge::FailTradeUnableToFindPartner:
        os << "Edge::FailTradeUnableToFindPartner";
        break;
    case Edge::MoveRobber:
        os << "Edge::MoveRobber";
        break;
    case Edge::OfferTrade:
        os << "Edge::OfferTrade";
        break;
    case Edge::PlayDevelopmentCard:
        os << "Edge::PlayDevelopmentCard";
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
