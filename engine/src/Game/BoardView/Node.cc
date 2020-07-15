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

void NodeView::for_each_hex_neighbor(const Callback<BoardView::Hex>& callback)
{
    for (int i = 0; i < 6; ++i) {
        const auto& direction = static_cast<Board::Direction>(i);
        auto* neighbor = m_neighbors.at(i);
        if (neighbor != nullptr && neighbor->type() == Type::Hex) {
            callback(direction, static_cast<Hex*>(neighbor));
        }
    }
}

void NodeView::for_each_junction_neighbor(const Callback<BoardView::Junction>& callback)
{
    for (int i = 0; i < 6; ++i) {
        const auto& direction = static_cast<Board::Direction>(i);
        auto* neighbor = m_neighbors.at(i);
        if (neighbor != nullptr && neighbor->type() == Type::Junction) {
            callback(direction, static_cast<Junction*>(neighbor));
        }
    }
}

void NodeView::for_each_road_neighbor(const Callback<BoardView::Road>& callback)
{
    for (int i = 0; i < 6; ++i) {
        const auto& direction = static_cast<Board::Direction>(i);
        auto* neighbor = m_neighbors.at(i);
        if (neighbor != nullptr && neighbor->type() == Type::Road) {
            callback(direction, static_cast<Road*>(neighbor));
        }
    }
}

void NodeView::for_each_hex_neighbor(const Callback<const BoardView::Hex>& callback) const
{
    for (int i = 0; i < 6; ++i) {
        const auto& direction = static_cast<Board::Direction>(i);
        const auto* neighbor = m_neighbors.at(i);
        if (neighbor != nullptr && neighbor->type() == Type::Hex) {
            callback(direction, static_cast<const Hex*>(neighbor));
        }
    }
}

void NodeView::for_each_junction_neighbor(const Callback<const BoardView::Junction>& callback) const
{
    for (int i = 0; i < 6; ++i) {
        const auto& direction = static_cast<Board::Direction>(i);
        const auto* neighbor = m_neighbors.at(i);
        if (neighbor != nullptr && neighbor->type() == Type::Junction) {
            callback(direction, static_cast<const Junction*>(neighbor));
        }
    }
}

void NodeView::for_each_road_neighbor(const Callback<const BoardView::Road>& callback) const
{
    for (int i = 0; i < 6; ++i) {
        const auto& direction = static_cast<Board::Direction>(i);
        const auto* neighbor = m_neighbors.at(i);
        if (neighbor != nullptr && neighbor->type() == Type::Road) {
            callback(direction, static_cast<const Road*>(neighbor));
        }
    }
}

NodeView* NodeView::decode(ByteBuffer& buf, const Board::Graph& graph)
{
    Decoder decoder(buf);
    bool is_null;
    if (!decoder.decode(is_null) || is_null)
        return nullptr;
    size_t index;
    if (!decoder.decode(index))
        return nullptr;
    const auto* node = graph.node(index);
    if (node == nullptr)
        return nullptr;
    NodeView::Type type;
    if (!decoder.decode(type))
        return nullptr;
    switch (type) {
    case NodeView::Type::Hex:
        return Hex::decode(buf, *node);
    case NodeView::Type::Junction:
        return Junction::decode(buf, *node);
    case NodeView::Type::Road:
        return Road::decode(buf, *node);
    default:
        return nullptr;
    };
}

} // namespace k10engine::Game::BoardView

using Type = k10engine::Game::BoardView::NodeView::Type;
using Hex = k10engine::Game::BoardView::Hex;
using Junction = k10engine::Game::BoardView::Junction;
using NodeView = k10engine::Game::BoardView::NodeView;
using Road = k10engine::Game::BoardView::Road;

template<>
void encode(ByteBuffer& buf, const Type& type)
{
    Encoder encoder(buf);
    encoder << static_cast<u8>(type);
}

template<>
bool decode(ByteBuffer& buf, Type& type)
{
    Decoder decoder(buf);
    u8 byte;
    if (!decoder.decode(byte))
        return false;
    type = static_cast<Type>(byte);
    return true;
}

template<>
void encode(ByteBuffer& buf, NodeView* const& node)
{
    Encoder encoder(buf);
    encoder << (node == nullptr);
    if (node == nullptr)
        return;
    encoder << node->index();
    encoder << static_cast<u8>(node->type());
    switch (node->type()) {
    case NodeView::Type::Hex:
        encoder << static_cast<const Hex&>(*node);
        break;
    case NodeView::Type::Junction:
        encoder << static_cast<const Junction&>(*node);
        break;
    case NodeView::Type::Road:
        encoder << static_cast<const Road&>(*node);
        break;
    default:
        assert(false);
    };
}
