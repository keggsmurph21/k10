#include "Core/Resource.h"
#include "Test/catch.h"

namespace k10engine {

TEST_CASE("Resource", "[Core][Core.Resource]")
{
    SECTION("Count comparing")
    {
        REQUIRE(!(ResourceCounts{} < ResourceCounts{}));

        REQUIRE(ResourceCounts{} < ResourceCounts{ { Resource::Brick, 1 } });
        REQUIRE(!(ResourceCounts{} < ResourceCounts{ { Resource::Brick, 0 } }));
        REQUIRE(
            !(ResourceCounts{ { Resource::Brick, 1 } } < ResourceCounts{ { Resource::Brick, 1 } }));
        REQUIRE(
            !(ResourceCounts{ { Resource::Brick, 0 } } < ResourceCounts{ { Resource::Brick, 0 } }));
        REQUIRE(
            !(ResourceCounts{ { Resource::Brick, 1 } } < ResourceCounts{ { Resource::Brick, 0 } }));
        REQUIRE(ResourceCounts{ { Resource::Brick, 0 } }
                < ResourceCounts{ { Resource::Brick, 1 } });

        REQUIRE(ResourceCounts{} < ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } });
        REQUIRE(
            !(ResourceCounts{} < ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 0 } }));
        REQUIRE(!(ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 0 } }
                  < ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 0 } }));
        REQUIRE(!(ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } }
                  < ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } }));
        REQUIRE(ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 1 } }
                < ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } });
        REQUIRE(ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 0 } }
                < ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } });
        REQUIRE(ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 0 } }
                < ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } });
        REQUIRE(ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 9 } }
                < ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } });
    }
}

} // namespace k10engine
