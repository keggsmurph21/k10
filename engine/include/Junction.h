#pragma once

#include "Node.h"

namespace Graph {

class Node;

class Junction : public Node {
public:
	Junction(int index) : m_index(index) { (void)m_index; }

private:
	int m_index;
};

}