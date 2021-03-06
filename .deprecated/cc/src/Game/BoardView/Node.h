#pragma once

#include <functional>

#include "Board/Graph.h"
#include "Board/Node.h"
#include "Forward.h"

namespace k10engine::Game::BoardView {

template<typename T>
using Neighbors = std::map<Board::Direction, T*, std::less<>>;

class NodeView {

    template<typename T>
    using Callback = std::function<void(const Board::Direction&, T*)>;

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

    virtual bool operator==(const NodeView&) const = 0;

    void for_each_hex_neighbor(const Callback<BoardView::Hex>&);
    void for_each_junction_neighbor(const Callback<BoardView::Junction>&);
    void for_each_road_neighbor(const Callback<BoardView::Road>&);

    void for_each_hex_neighbor(const Callback<const BoardView::Hex>&) const;
    void for_each_junction_neighbor(const Callback<const BoardView::Junction>&) const;
    void for_each_road_neighbor(const Callback<const BoardView::Road>&) const;

    static NodeView* decode(ByteBuffer&, const Board::Graph&);

private:
    const Board::Node& m_node;
    const Type m_type;

protected:
    NodeView(const Board::Node&, Type);
    std::vector<NodeView*> m_neighbors;
};

} // namespace k10engine::Game::BoardView

template<>
void encode(ByteBuffer&, const k10engine::Game::BoardView::NodeView::Type&);

template<>
bool decode(ByteBuffer&, k10engine::Game::BoardView::NodeView::Type&);

template<>
void encode(ByteBuffer&, k10engine::Game::BoardView::NodeView* const&);
