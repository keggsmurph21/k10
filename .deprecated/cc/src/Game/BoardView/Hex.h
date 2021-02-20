#pragma once

#include <iostream>

#include "Core/Resource.h"
#include "Forward.h"
#include "Game/BoardView/Node.h"

namespace k10engine::Game::BoardView {

class Hex : public NodeView {
public:
    const AbstractResource& resource() const { return m_resource; }
    size_t roll_number() const { return m_roll_number; }

    Hex(const Board::Node& node, AbstractResource resource, size_t roll_number)
        : NodeView(node, NodeView::Type::Hex)
        , m_resource(resource)
        , m_roll_number(roll_number)
    {
    }
    static Hex* decode(ByteBuffer&, const Board::Node&);

    friend std::ostream& operator<<(std::ostream&, const Hex&);
    virtual bool operator==(const NodeView&) const override;

private:
    const AbstractResource m_resource;
    size_t m_roll_number; // 0 means <none>
};

} // namespace k10engine::Game::BoardView

template<>
void encode(ByteBuffer&, const k10engine::Game::BoardView::Hex&);
