#pragma once

#include "Node.h"

namespace Graph {

class Node;

class Flippable : public Node {
public:
	Flippable(int index) : m_index(index) { (void)m_index; }

private:
	int m_index;
};

}