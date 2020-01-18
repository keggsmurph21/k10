#pragma once

#include <map>
#include <string.h>
#include <utility>
#include <vector>

#include "Board/Direction.h"
#include "Board/Graph.h"
#include "Board/Orientation.h"
#include "Board/Port.h"

namespace k10engine {

namespace Board {

class Base {
public:
    Base(const std::string name,
         const Graph*,
         const std::vector<std::tuple<int, int, Orientation>>);
    ~Base();

    std::string name() const { return m_name; }
    const Graph* graph() const { return m_graph; }
    const std::vector<Port*> ports() const { return m_ports; }

    // static bool has(std::string name);
    // static Base* get(std::string name);

private:
    const std::string m_name;
    const Graph* m_graph;
    std::vector<Port*> m_ports;
};

} // namespace Board

} // namespace k10engine
