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

    Flippable* flippable(int);
    int n_flippables() const { return m_flippables.size(); }

    Hex* hex(int);
    int n_hexes() const { return m_hexes.size(); }

    Junction* junction(int);
    int n_junctions() const { return m_junctions.size(); }

    Ocean* ocean(int);
    int n_oceans() const { return m_oceans.size(); }

    Road* road(int);
    int n_roads() const { return m_roads.size(); }

private:
    std::vector<Flippable*> m_flippables;
    std::vector<Hex*> m_hexes;
    std::vector<Junction*> m_junctions;
    std::vector<Ocean*> m_oceans;
    std::vector<Road*> m_roads;
};

} // namespace Board

} // namespace k10engine
