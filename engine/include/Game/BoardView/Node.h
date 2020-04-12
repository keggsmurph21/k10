#pragma once

#include <functional>

#include "Board/Graph.h"
#include "Board/Node.h"
#include "Forward.h"

namespace k10engine::Game::BoardView {

template<typename T>
using Neighbors = std::map<Board::Direction, T*, std::less<>>;

class NodeView {
public:
    enum class Type {
        Hex,
        Junction,
        Road,
    };

    const Board::Node& node() const { return m_node; }
    size_t index() const { return m_node.index(); }
    const Type& type() const { return m_type; }

    std::vector<NodeView*>& neighbors() { return m_neighbors; }
    const std::vector<NodeView*>& neighbors() const { return m_neighbors; }
    void add_neighbor_(Board::Direction direction, NodeView* neighbor)
    {
        m_neighbors[static_cast<size_t>(direction)] = neighbor;
    }

    bool is_hex() const;
    bool is_junction() const;
    bool is_road() const;

    virtual ~NodeView() {}

    virtual bool operator==(const NodeView&) const;

    void
    for_each_hex_neighbor(const std::function<void(const Board::Direction&, BoardView::Hex*)>&);
    void for_each_junction_neighbor(
        const std::function<void(const Board::Direction&, BoardView::Junction*)>&);
    void
    for_each_road_neighbor(const std::function<void(const Board::Direction&, BoardView::Road*)>&);

    void for_each_hex_neighbor(
        const std::function<void(const Board::Direction&, const BoardView::Hex*)>&) const;
    void for_each_junction_neighbor(
        const std::function<void(const Board::Direction&, const BoardView::Junction*)>&) const;
    void for_each_road_neighbor(
        const std::function<void(const Board::Direction&, const BoardView::Road*)>&) const;

private:
    const Board::Node& m_node;
    const Type m_type;

protected:
    NodeView(const Board::Node&, Type);
    std::vector<NodeView*> m_neighbors;
};

} // namespace k10engine::Game::BoardView
