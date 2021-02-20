#pragma once

#include <string.h>

#include "Board/Direction.h"
#include "Board/Graph.h"

namespace k10engine::Board {

const Graph* from_string(const std::string& board_string);
const Graph* from_file(const std::string& filename);

} // namespace k10engine::Board
