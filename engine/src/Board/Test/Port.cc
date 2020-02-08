#include <stdexcept>

#include "Board/Port.h"
#include "Test/catch.h"

namespace k10engine {

namespace Board {

TEST_CASE("Port initialization", "[Board][Board.Port]")
{
    auto o = Orientation::Clock2Clock8;
}

} // namespace Board

} // namespace k10engine
