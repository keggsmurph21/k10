#include <stdio.h>
#include "Graph.h"
#include <unistd.h>
//#include "Node.h"

using namespace Graph;

int main(int, char**)
{
    UndirectedGraph g(10);
    g.add_edge(2, Direction::Clock4, 3);
    assert(g[2]->has_edge(Direction::Clock4));
    assert(g[3]->has_edge(Direction::Clock10));
    g.add_edge(2, Direction::Clock6, 5);
    assert(g[2]->has_edge(Direction::Clock6));
    assert(g[5]->has_edge(Direction::Clock12));
    g.add_edge(3, Direction::Clock4, 7);
    assert(g[3]->has_edge(Direction::Clock4));
    assert(g[7]->has_edge(Direction::Clock10));
    g.add_edge(5, Direction::Clock2, 7);
    assert(g[5]->has_edge(Direction::Clock2));
    assert(g[7]->has_edge(Direction::Clock8));
    g.dump();
    sleep(100);
    printf("hello world\n");
    /*
    (HexNode(14, HexNode::ResourceType::Normal)).dump();
    (HexNode(1, HexNode::ResourceType::Flippable)).dump();
    (HexNode(6, HexNode::ResourceType::Ocean)).dump();
    (JunctionNode(9)).dump();

    auto h = HexNode(420, HexNode::ResourceType::Normal);
    h.dump();
    for (auto it : h) {
        printf(".");
    }
    printf("\n");
    */
}
