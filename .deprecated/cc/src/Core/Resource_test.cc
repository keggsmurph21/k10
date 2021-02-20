#include "Core/Resource.h"
#include "Test/catch.h"

namespace k10engine {

TEST_CASE("Resource", "[Core][Core.Resource]")
{
    SECTION("Count comparing")
    {
        REQUIRE(!(ResourceCounts{} < ResourceCounts{}));
        REQUIRE(ResourceCounts{} == ResourceCounts{});

        REQUIRE(ResourceCounts{} < ResourceCounts{ { Resource::Brick, 1 } });
        REQUIRE(!(ResourceCounts{} == ResourceCounts{ { Resource::Brick, 1 } }));
        REQUIRE(!(ResourceCounts{} < ResourceCounts{ { Resource::Brick, 0 } }));
        REQUIRE(ResourceCounts{} == ResourceCounts{ { Resource::Brick, 0 } });
        REQUIRE(!(ResourceCounts{ { Resource::Brick, 1 } } < ResourceCounts{ { Resource::Brick, 1 } }));
        REQUIRE(ResourceCounts{ { Resource::Brick, 1 } } == ResourceCounts{ { Resource::Brick, 1 } });
        REQUIRE(!(ResourceCounts{ { Resource::Brick, 0 } } < ResourceCounts{ { Resource::Brick, 0 } }));
        REQUIRE(ResourceCounts{ { Resource::Brick, 0 } } == ResourceCounts{ { Resource::Brick, 0 } });
        REQUIRE(!(ResourceCounts{ { Resource::Brick, 1 } } < ResourceCounts{ { Resource::Brick, 0 } }));
        REQUIRE(!(ResourceCounts{ { Resource::Brick, 1 } } == ResourceCounts{ { Resource::Brick, 0 } }));
        REQUIRE(ResourceCounts{ { Resource::Brick, 0 } } < ResourceCounts{ { Resource::Brick, 1 } });
        REQUIRE(!(ResourceCounts{ { Resource::Brick, 0 } } == ResourceCounts{ { Resource::Brick, 1 } }));

        REQUIRE(ResourceCounts{} < ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } });
        REQUIRE(!(ResourceCounts{} == ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } }));
        REQUIRE(!(ResourceCounts{} < ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 0 } }));
        REQUIRE(ResourceCounts{} == ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 0 } });
        REQUIRE(!(ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 0 } }
                  < ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 0 } }));
        REQUIRE(ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 0 } }
                == ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 0 } });
        REQUIRE(!(ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } }
                  < ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } }));
        REQUIRE(ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } }
                == ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } });
        REQUIRE(ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 1 } }
                < ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } });
        REQUIRE(!(ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 1 } }
                  == ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } }));
        REQUIRE(ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 0 } }
                < ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } });
        REQUIRE(!(ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 0 } }
                  == ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } }));
        REQUIRE(ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 0 } }
                < ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } });
        REQUIRE(!(ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 0 } }
                  == ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } }));
        REQUIRE(ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 9 } }
                < ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } });
        REQUIRE(!(ResourceCounts{ { Resource::Brick, 0 }, { Resource::Ore, 9 } }
                  == ResourceCounts{ { Resource::Brick, 1 }, { Resource::Ore, 1 } }));
    }

    SECTION("Serialization")
    {
        SECTION("Resource")
        {
            ByteBuffer buf;
            Encoder encoder(buf);
            Resource expected{ Resource::Wheat };
            encoder << expected;

            Decoder decoder(buf);
            Resource actual;
            REQUIRE(decoder.decode(actual));
            REQUIRE(actual == expected);
        }

        SECTION("NonYieldingResource")
        {
            ByteBuffer buf;
            Encoder encoder(buf);
            NonYieldingResource expected{ NonYieldingResource::Desert };
            encoder << expected;

            Decoder decoder(buf);
            NonYieldingResource actual;
            REQUIRE(decoder.decode(actual));
            REQUIRE(actual == expected);
        }

        SECTION("AbstractResource")
        {
            ByteBuffer buf;

            {
                // with a Resource
                Encoder encoder(buf);
                AbstractResource expected{ Resource::Wheat };
                encoder << expected;

                Decoder decoder(buf);
                AbstractResource actual;
                REQUIRE(decoder.decode(actual));
                REQUIRE(actual == expected);
            }

            {
                // with a NonYieldingResource
                Encoder encoder(buf);
                AbstractResource expected{ NonYieldingResource::Desert };
                encoder << expected;

                Decoder decoder(buf);
                AbstractResource actual;
                REQUIRE(decoder.decode(actual));
                REQUIRE(actual == expected);
            }
        }

        SECTION("ResourceCollection")
        {
            ByteBuffer buf;
            Encoder encoder(buf);

            {
                // empty
                Encoder encoder(buf);
                ResourceCollection expected = {};
                encoder << expected;

                Decoder decoder(buf);
                ResourceCollection actual;
                REQUIRE(decoder.decode(actual));
                REQUIRE(actual == expected);
            }

            {
                // some stuff
                Encoder encoder(buf);
                ResourceCollection expected = {
                    Resource::Ore, Resource::Sheep, Resource::Brick, Resource::Wood, Resource::Wheat
                };
                encoder << expected;

                Decoder decoder(buf);
                ResourceCollection actual;
                REQUIRE(decoder.decode(actual));
                REQUIRE(actual == expected);
            }
        }
    }
}

} // namespace k10engine
