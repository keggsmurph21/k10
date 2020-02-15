#include "Core/Edge.h"

namespace k10engine::State {

std::ostream& operator<<(std::ostream& os, Edge edge)
{
    switch (edge) {
    case Edge::AcceptTrade:
        os << "Edge::AcceptTrade";
        break;
    case Edge::AcceptTradeOther:
        os << "Edge::AcceptTradeOther";
        break;
    case Edge::AfterTradeOther:
        os << "Edge::AfterTradeOther";
        break;
    case Edge::Build:
        os << "Edge::Build";
        break;
    case Edge::CancelTrade:
        os << "Edge::CancelTrade";
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
    case Edge::EndGame:
        os << "Edge::EndGame";
        break;
    case Edge::EndInit:
        os << "Edge::EndInit";
        break;
    case Edge::FailTradeUnableToFindPartner:
        os << "Edge::FailTradeUnableToFindPartner";
        break;
    case Edge::InitBuildFirstRoad:
        os << "Edge::InitBuildFirstRoad";
        break;
    case Edge::InitBuildSecondRoad:
        os << "Edge::InitBuildSecondRoad";
        break;
    case Edge::InitCollectResources:
        os << "Edge::InitCollectResources";
        break;
    case Edge::InitSettle:
        os << "Edge::InitSettle";
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
    case Edge::TakeTurn:
        os << "Edge::TakeTurn";
        break;
    case Edge::ToRoot:
        os << "Edge::ToRoot";
        break;
    case Edge::TradeBank:
        os << "Edge::TradeBank";
        break;
    }
    return os;
}

} // namespace k10engine::State
