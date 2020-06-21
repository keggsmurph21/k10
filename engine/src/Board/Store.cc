#include <cassert>

#include "Board/Parser.h"
#include "Board/Store.h"

namespace k10engine::Board {

static Store* s_board_store = nullptr;

const Store& Store::the()
{
    if (s_board_store == nullptr)
        s_board_store = new Store();
    return *s_board_store;
}

Store::Store()
{
    m_named_boards.reserve(5);
    m_named_boards[static_cast<size_t>(Name::Portless)] = from_file("static/Portless.board");
    m_named_boards[static_cast<size_t>(Name::Single)] = from_file("static/Single.board");
    m_named_boards[static_cast<size_t>(Name::Standard)] = from_file("static/Standard.board");
    m_named_boards[static_cast<size_t>(Name::Tall)] = from_file("static/Tall.board");
    m_named_boards[static_cast<size_t>(Name::Triple)] = from_file("static/Triple.board");
}

Store::~Store()
{
    for (auto* named_board : m_named_boards)
        delete named_board;
}

const Graph* Store::by_name(const Name& name) const
{
    switch (name) {
    case Name::Portless:
    case Name::Single:
    case Name::Standard:
    case Name::Tall:
    case Name::Triple:
        return m_named_boards[static_cast<size_t>(name)];
    default:
        assert(false);
    }
}

Name Store::name_of(const Graph* board) const
{
    for (size_t i = 0; i < m_named_boards.size(); ++i) {
        const auto* named_board = m_named_boards.at(i);
        if (named_board == board)
            return static_cast<Name>(i);
    }
    assert(false);
}

} // namespace k10engine::Board
