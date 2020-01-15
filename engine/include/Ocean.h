#pragma once

#include "Node.h"

namespace Graph {

class Node;

class Ocean : public Node {
public:
	Ocean(int index) : m_index(index) { (void)m_index; }

private:
	int m_index;
};

}