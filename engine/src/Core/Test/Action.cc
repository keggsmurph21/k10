#include "Core/Action.h"
#include "Core/Building.h"
#include "Core/Resource.h"
#include "Test/catch.h"

namespace k10engine {

TEST_CASE("Action", "[Core.Action]")
{
    SECTION("(de)serialization")
    {
        auto test_roundtrip = [](const Game::Action& expected_action) {
            Game::Action actual_action;

            ByteBuffer buf;
            Encoder encoder(buf);
            encoder << expected_action;
            Decoder decoder(buf);

            REQUIRE(decoder.decode(actual_action));

            REQUIRE(actual_action.edge == expected_action.edge);
            REQUIRE(actual_action.args.size() == expected_action.args.size());
            for (auto i = 0; i < expected_action.args.size(); ++i) {
                auto& actual_arg = actual_action.args.at(i);
                auto& expected_arg = expected_action.args.at(i);
                REQUIRE(actual_arg.type == expected_arg.type);
                REQUIRE(actual_arg.value == expected_arg.value);
            }
        };

        test_roundtrip({ State::Edge::BuildSettlement, { { Game::ActionArgumentType::NodeId, 42 } } });
        test_roundtrip({ State::Edge::OfferTrade,
                         { { Game::ActionArgumentType::GiveResourceType, static_cast<size_t>(Resource::Brick) },
                           { Game::ActionArgumentType::ResourceCount, 1 },
                           { Game::ActionArgumentType::TakeResourceType, static_cast<size_t>(Resource::Sheep) },
                           { Game::ActionArgumentType::ResourceCount, 2 } } });
    }
}

} // namespace k10engine
