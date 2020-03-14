#include <cassert>
#include <fstream>
#include <istream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "Board/Parser.h"

namespace k10engine::Board {

struct NeighborSpec {
    Direction direction;
    int dx;
    int dy;
};

struct Token {
    int num{ -1 };
    char ch{ ' ' };
    int x{ -1 };
    int y{ -1 };
};

static NodeType node_type(char ch)
{
    switch (ch) {
    case '.':
    case 'p':
        return NodeType::Junction;
    case 'o':
        return NodeType::Ocean;
    case 'h':
        return NodeType::Hex;
    case '?':
        return NodeType::UnflippedHex;
    case '|':
    case '/':
    case '\\':
        return NodeType::Road;
    default:
        assert(false);
    }
}

static std::vector<NeighborSpec> junctionlike_neighbor_specs = {
    { Direction::Clock2, 2, -1 }, { Direction::Clock4, 2, 1 },    { Direction::Clock6, 0, 1 },
    { Direction::Clock8, -2, 1 }, { Direction::Clock10, -2, -1 }, { Direction::Clock12, 0, -1 },
    { Direction::Clock2, 4, -1 }, { Direction::Clock4, 4, 1 },    { Direction::Clock6, 0, 3 },
    { Direction::Clock8, -4, 1 }, { Direction::Clock10, -4, -1 }, { Direction::Clock12, 0, -3 },
};
static std::vector<NeighborSpec> hexlike_neighbor_specs = {
    { Direction::Clock2, 4, -1 }, { Direction::Clock4, 4, 1 },    { Direction::Clock6, 0, 3 },
    { Direction::Clock8, -4, 1 }, { Direction::Clock10, -4, -1 }, { Direction::Clock12, 0, -3 },
};
static std::vector<NeighborSpec> vertical_road_neighbor_specs = {
    { Direction::Clock6, 0, 1 },
    { Direction::Clock12, 0, -1 },
};
static std::vector<NeighborSpec> forwardslash_road_neighbor_specs = {
    { Direction::Clock2, 2, -1 },
    { Direction::Clock8, -2, 1 },
};
static std::vector<NeighborSpec> backslash_road_neighbor_specs = {
    { Direction::Clock4, 2, 1 },
    { Direction::Clock10, -2, -1 },
};
static std::unordered_map<char, const std::vector<NeighborSpec>*> neighbor_specs = {
    { '.', &junctionlike_neighbor_specs },      { 'p', &junctionlike_neighbor_specs },
    { 'o', &hexlike_neighbor_specs },           { 'h', &hexlike_neighbor_specs },
    { '?', &hexlike_neighbor_specs },           { '|', &vertical_road_neighbor_specs },
    { '/', &forwardslash_road_neighbor_specs }, { '\\', &backslash_road_neighbor_specs },
};

static Graph from_stream(std::istream& char_stream)
{
    NodeSpecs nodes;
    EdgeSpecs edges;
    PortSpecs ports;

    char ch;

    // Calculate dimensions
    int height = 0;
    int line_width = 0;
    int max_width = 0;
    while (char_stream.get(ch)) {
        switch (ch) {
        case '\n':
            ++height;
            if (line_width > max_width) {
                max_width = line_width;
            }
            line_width = 0;
            break;
        default:
            ++line_width;
        }
    }
    char_stream.clear();
    char_stream.seekg(0);

    // Initialize the token_matrix
    auto token_matrix = std::vector<std::vector<Token>>(height);
    for (int i = 0; i < height; ++i) {
        token_matrix[i] = std::vector<Token>(max_width);
    }

    // Build up the nodes and token_matrix
    Token token;
    int num = 0;
    int x = 0;
    int y = 0;
    while (char_stream.get(ch)) {
        switch (ch) {
        case '\n':
            x = 0;
            ++y;
            continue;
        case ' ':
            ++x;
            continue;
        case '.':
        case 'p':
        case 'o':
        case 'h':
        case '?':
        case '|':
        case '/':
        case '\\':
            token = { num, ch, x, y };
            break;
        default:
            assert(false);
        }
        token_matrix[y][x] = token;
        nodes.push_back({ static_cast<size_t>(x), static_cast<size_t>(y), node_type(token.ch) });
        ++num;
        ++x;
    }

    // Build up the neighbors
    for (const auto& token_row : token_matrix) {
        for (const auto& token : token_row) {
            if (token.num == -1) {
                continue;
            }
            for (const auto& neighbor_spec : *neighbor_specs.at(token.ch)) {
                const auto& direction = neighbor_spec.direction;
                const auto& dx = neighbor_spec.dx;
                const auto& dy = neighbor_spec.dy;
                if (0 > token.x + dx || token.x + dx >= max_width) {
                    continue;
                }
                if (0 > token.y + dy || token.y + dy >= height) {
                    continue;
                }
                const auto& neighbor = token_matrix[token.y + dy][token.x + dx];
                if (neighbor.num == -1) {
                    continue;
                }
                edges.push_back({ static_cast<size_t>(token.num),
                                  static_cast<size_t>(neighbor.num),
                                  direction });

                // Build up the ports
                if (token.ch == 'p') {
                    if (dx < -2 || 2 < dx || dy < -2 || 2 < dy) {
                        continue;
                    }
                    if (0 > token.x + 2 * dx || token.x + 2 * dx >= max_width) {
                        continue;
                    }
                    if (0 > token.y + 2 * dy || token.y + 2 * dy >= height) {
                        continue;
                    }
                    const auto& second_neighbor = token_matrix[token.y + 2 * dy][token.x + 2 * dx];
                    if (second_neighbor.num == -1) {
                        continue;
                    }
                    if (second_neighbor.ch != 'p') {
                        continue;
                    }
                    if (token.num > second_neighbor.num) {
                        continue;
                    }
                    ports.push_back({ static_cast<size_t>(token.num),
                                      static_cast<size_t>(second_neighbor.num),
                                      get_orientation(direction) });
                }
            }
        }
    }

    return Graph(
        { static_cast<size_t>(max_width), static_cast<size_t>(height) }, nodes, edges, ports);
}

Graph from_string(const std::string& board_string)
{
    std::stringstream board_sstream(board_string);
    return from_stream(board_sstream);
}

Graph from_file(const std::string& filename)
{
    std::ifstream board_fstream(filename);
    return from_stream(board_fstream);
}

} // namespace k10engine::Board
