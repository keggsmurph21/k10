#pragma once

#include "Node.h"

namespace Graph {

class Node;

class Hex : public Node {
public:
	Hex(int index) : m_index(index) { (void)m_index; }

private:
	int m_index;
};

}