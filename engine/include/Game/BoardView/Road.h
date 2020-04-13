#pragma once

#include <iostream>

#include "Core/Resource.h"
#include "Forward.h"
#include "Game/BoardView/Node.h"

namespace k10engine::Game::BoardView {

class Road : public NodeView {
public:
    const Player* owner() const { return m_owner; }
    void set_owner(Player* owner) { m_owner = owner; }

    Road(const Board::Node& node)
        : NodeView(node, NodeView::Type::Road)
    {
    }

    friend std::ostream& operator<<(std::ostream&, const Road&);

    virtual bool operator==(const NodeView&) const override;

    virtual std::vector<u8> serialize() const override;
    static Road deserialize(const Board::Node&, const std::vector<u8>&);

private:
    Player* m_owner{ nullptr };
};

} // namespace k10engine::Game::BoardView
