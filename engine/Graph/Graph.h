#pragma once

#include <iostream>
#include <vector>

#include "Road.h"
#include "Junction.h"
#include "Hex.h"
#include "Flippable.h"
#include "Ocean.h"

namespace Graph {

class Road;
class Junction;
class Hex;
class Flippable;
class Ocean;

enum class Direction {
    Clock12,
    Clock2,
    Clock4,
    Clock6,
    Clock8,
    Clock10,
};
Direction get_opposite(Direction);

class Graph {
public:
    Graph();
    Graph(int, int, int, int, int);
    ~Graph();

    friend std::ostream& operator<<(std::ostream&, Graph&);

    Road* road(int);
    Junction* junction(int);
    Hex* hex(int);
    Flippable* flippable(int);
    Ocean* ocean(int);
    
private:
    std::vector<Road*> m_roads;
    std::vector<Junction*> m_junctions;
    std::vector<Hex*> m_hexes;
    std::vector<Flippable*> m_flippables;
    std::vector<Ocean*> m_oceans;
};

}
