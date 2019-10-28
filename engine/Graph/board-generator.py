import os

from collections import Counter
from collections import defaultdict

# fstrings can't have backslashes ...
NL = '\n'
OB = '{'
CB = '}'

class Position:
    def __init__(self, num, dx, dy):
        self.num = num
        self.dx = dx
        self.dy = dy

    def __repr__(self):
        return f'Clock{self.num}'

    def generate_cpp(self):
        return f'Direction::Clock{self.num}'

class Node:
    char = None
    def __init__(self, num, x, y):
        self.num = num
        self.x = x
        self.y = y
        self.neighbors = {
            Position(12, -3, 0): None,
            Position(2, -1, 4): None,
            Position(4, 1, 4): None,
            Position(6, 3, 0): None,
            Position(8, 1, -4): None,
            Position(10, -1, -4): None,
        }

    def __iter__(self):
        for pos, neighbor in self.neighbors.items():
            if neighbor is not None:
                yield (pos, neighbor)

    def __len__(self):
        return len([ _ for _ in self ])

    def __repr__(self):
        return f'{self.char} #{self.num} ({self.x},{self.y})'

class Whitespace(Node):
    char = ' '

class Ocean(Node):
    char = 'o'
    def generate_cpp(self):
        for pos, neighbor in self:
            yield f'm_graph->ocean({self.num})->add_edge({pos.generate_cpp()}, m_graph->junction({neighbor.num}));'

class Hex(Node):
    char = 'h'
    def generate_cpp(self):
        for pos, neighbor in self:
            yield f'm_graph->hex({self.num})->add_edge({pos.generate_cpp()}, m_graph->junction({neighbor.num}));'

class Flippable(Node):
    char = '?'
    def generate_cpp(self):
        for pos, neighbor in self:
            yield f'm_graph->flippable({self.num})->add_edge({pos.generate_cpp()}, m_graph->junction({neighbor.num}));'

class Junction(Node):
    char = '.'
    def __init__(self, num, x, y):
        super().__init__(num, x, y)
        self.neighbors.update({
            Position(12, -1, 0): None,
            Position(6, 1, 0): None,
            Position(2, -1, 2): None,
            Position(8, 1, -2): None,
            Position(4, 1, 2): None,
            Position(10, -1, -2): None,
        })

    def generate_cpp(self):
        for pos, neighbor in self:
            if isinstance(neighbor, Ocean):
                yield f'm_graph->junction({self.num})->add_edge({pos.generate_cpp()}, m_graph->ocean({neighbor.num}));'
            elif isinstance(neighbor, Hex):
                yield f'm_graph->junction({self.num})->add_edge({pos.generate_cpp()}, m_graph->hex({neighbor.num}));'
            elif isinstance(neighbor, Flippable):
                yield f'm_graph->junction({self.num})->add_edge({pos.generate_cpp()}, m_graph->flippable({neighbor.num}));'
            elif isinstance(neighbor, Road):
                yield f'm_graph->junction({self.num})->add_edge({pos.generate_cpp()}, m_graph->road({neighbor.num}));'

class Road(Node):
    def __init__(self, num, x, y, orientation):
        super().__init__(num, x, y)
        self.char = self.orientation = orientation
        if self.orientation == '|':
            self.neighbors = {
                Position(12, -1, 0): None,
                Position(6, 1, 0): None,
            }
        elif self.orientation == '/':
            self.neighbors = {
                Position(2, -1, 2): None,
                Position(8, 1, -2): None,
            }
        elif self.orientation == '\\':
            self.neighbors = {
                Position(4, 1, 2): None,
                Position(10, -1, -2): None,
            }

    def generate_cpp(self):
        for pos, neighbor in self:
            yield f'm_graph->road({self.num})->add_edge({pos.generate_cpp()}, m_graph->junction({neighbor.num}));'

class Board:
    def __init__(self, name):

        self.name = name
        self.text_file = os.path.join('Boards', f'{name}.txt')
        self.cpp_file = os.path.join('Generated', f'{name}.h')

        self.junctions = []
        self.roads = []
        self.hexes = []
        self.flippables = []
        self.oceans = []

        with open(self.text_file) as fp:

            # initialize arr
            self.height = 0
            self.width = 0
            for line in fp:
                self.height += 1
                self.width = max(self.width, len(line))
            self.arr = [ [ Whitespace(-1, i, j) for j in range(self.width) ] for i in range(self.height) ]

            # lex
            fp.seek(0)
            for x, line in enumerate(fp):
                for y, ch in enumerate(line):
                    if ch in (' ', '\n'):
                        continue
                    elif ch == '.':
                        token = Junction(len(self.junctions), x, y)
                        self.junctions.append(token)
                    elif ch == 'o':
                        token = Ocean(len(self.oceans), x, y)
                        self.oceans.append(token)
                    elif ch == 'h':
                        token = Hex(len(self.hexes), x, y)
                        self.hexes.append(token)
                    elif ch == '?':
                        token = Flippable(len(self.flippables), x, y)
                        self.flippables.append(token)
                    elif ch in ('|', '/', '\\'):
                        token = Road(len(self.roads), x, y, ch)
                        self.roads.append(token)
                    else:
                        assert False
                    self.arr[token.x][token.y] = token

        '''
        # check lexing
        assert len(self.junctions) == 54
        assert len(self.roads) == 72
        assert len(self.hexes) == 19
        assert len(self.flippables) == 0
        assert len(self.oceans) == 18
        '''

        # parse
        for token_list in [
                self.roads,
                self.junctions,
                self.hexes,
                self.flippables,
                self.oceans ]:
            for token in token_list:
                for pos in token.neighbors:
                    off_x = token.x + pos.dx
                    if off_x < 0 or self.height <= off_x:
                        continue
                    off_y = token.y + pos.dy
                    if off_y < 0 or self.width <= off_y:
                        continue
                    neighbor = self.arr[off_x][off_y]
                    if isinstance(neighbor, Whitespace):
                        continue
                    token.neighbors[pos] = neighbor

        '''
        # check parsing
        for token in self.roads:
            assert len(token) == 2
        for token in self.junctions:
            assert len(token) in (5, 6)
        for token in self.hexes:
            assert len(token) == 6
        for token in self.flippables:
            assert len(token) == 0
        for token in self.oceans:
            assert len(token) in (2, 3)
        '''

    def generate_cpp(self):
        with open(self.cpp_file, 'w') as fp:
            fp.write(f'''
#pragma once

#include "../Board.h"
#include "../Graph.h"

namespace Graph {OB}

class {self.name}Board : public Board {OB}
public:
    {self.name}Board()
    {OB}
        m_graph = new Graph(
            {len(self.roads)}, // roads
            {len(self.junctions)}, // junctions
            {len(self.hexes)}, // hexes
            {len(self.flippables)}, // flippables
            {len(self.oceans)} // oceans
        );

{NL.join([ NL.join(r.generate_cpp()) for r in self.roads ])}
{NL.join([ NL.join(j.generate_cpp()) for j in self.junctions ])}
{NL.join([ NL.join(h.generate_cpp()) for h in self.hexes ])}
{NL.join([ NL.join(f.generate_cpp()) for f in self.flippables ])}
{NL.join([ NL.join(o.generate_cpp()) for o in self.oceans ])}
    {CB}

private:
    Graph* m_graph;

{CB};

{CB}
''')

if __name__ == '__main__':

    from argparse import ArgumentParser

    parser = ArgumentParser()
    parser.add_argument('name')
    args = parser.parse_args()

    b = Board(args.name)
    b.generate_cpp()
