#include "Game/BoardView/Node.h"
#include "Board/Graph.h"
#include "Game/BoardView/Hex.h"
#include "Game/BoardView/Junction.h"
#include "Game/BoardView/Road.h"

namespace k10engine::Game::BoardView {

NodeView::NodeView(const Board::Node& node, Type type)
    : m_node(node)
    , m_type(type)
{
    m_neighbors.reserve(6);
    for (int i = 0; i < 6; ++i) {
        m_neighbors.push_back(nullptr);
    }
}

bool NodeView::is_hex() const
{
    return type() == Type::Hex;
}

bool NodeView::is_junction() const
{
    return type() == Type::Junction;
}

bool NodeView::is_road() const
{
    return type() == Type::Road;
}

void NodeView::for_each_hex_neighbor(
    const std::function<void(const Board::Direction&, BoardView::Hex*)>& callback)
{
    for (int i = 0; i < 6; ++i) {
        const auto& direction = static_cast<Board::Direction>(i);
        auto* neighbor = m_neighbors.at(i);
        if (neighbor != nullptr && neighbor->type() == Type::Hex) {
            callback(direction, static_cast<Hex*>(neighbor));
        }
    }
}

void NodeView::for_each_junction_neighbor(
    const std::function<void(const Board::Direction&, BoardView::Junction*)>& callback)
{
    for (int i = 0; i < 6; ++i) {
        const auto& direction = static_cast<Board::Direction>(i);
        auto* neighbor = m_neighbors.at(i);
        if (neighbor != nullptr && neighbor->type() == Type::Junction) {
            callback(direction, static_cast<Junction*>(neighbor));
        }
    }
}

void NodeView::for_each_road_neighbor(
    const std::function<void(const Board::Direction&, BoardView::Road*)>& callback)
{
    for (int i = 0; i < 6; ++i) {
        const auto& direction = static_cast<Board::Direction>(i);
        auto* neighbor = m_neighbors.at(i);
        if (neighbor != nullptr && neighbor->type() == Type::Road) {
            callback(direction, static_cast<Road*>(neighbor));
        }
    }
}

void NodeView::for_each_hex_neighbor(
    const std::function<void(const Board::Direction&, const BoardView::Hex*)>& callback) const
{
    for (int i = 0; i < 6; ++i) {
        const auto& direction = static_cast<Board::Direction>(i);
        const auto* neighbor = m_neighbors.at(i);
        if (neighbor != nullptr && neighbor->type() == Type::Hex) {
            callback(direction, static_cast<const Hex*>(neighbor));
        }
    }
}

void NodeView::for_each_junction_neighbor(
    const std::function<void(const Board::Direction&, const BoardView::Junction*)>& callback) const
{
    for (int i = 0; i < 6; ++i) {
        const auto& direction = static_cast<Board::Direction>(i);
        const auto* neighbor = m_neighbors.at(i);
        if (neighbor != nullptr && neighbor->type() == Type::Junction) {
            callback(direction, static_cast<const Junction*>(neighbor));
        }
    }
}

void NodeView::for_each_road_neighbor(
    const std::function<void(const Board::Direction&, const BoardView::Road*)>& callback) const
{
    for (int i = 0; i < 6; ++i) {
        const auto& direction = static_cast<Board::Direction>(i);
        const auto* neighbor = m_neighbors.at(i);
        if (neighbor != nullptr && neighbor->type() == Type::Road) {
            callback(direction, static_cast<const Road*>(neighbor));
        }
    }
}

} // namespace k10engine::Game::BoardView
