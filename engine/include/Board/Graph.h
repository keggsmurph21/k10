#pragma once

#include <iostream>
#include <vector>

#include "Board/Direction.h"
#include "Board/Flippable.h"
#include "Board/Hex.h"
#include "Board/Junction.h"
#include "Board/Ocean.h"
#include "Board/Orientation.h"
#include "Board/Road.h"

namespace k10engine {

namespace Board {

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

} // namespace Board

} // namespace k10engine
