#include <cassert>
#include <iostream>

#include "Scenario/Store.h"
#include "Server/Request.h"
#include "Test/catch.h"
#include "Util/ByteBuffer.h"
#include "Util/Decoder.h"
#include "Util/Types.h"

#define BYTE_FOR(x) (static_cast<u8>(Request::Type::x))
#define SEQN_NUMBER 0x04, 0x20

namespace k10engine::Server {

template<typename T>
const T* decode(const std::vector<u8>& bytes)
{
    ByteBuffer buf;
    for (auto byte : bytes)
        buf.append(byte);
    Decoder decoder(buf);
    Request* request = nullptr;
    if (!decoder.decode(request))
        return nullptr;
    return static_cast<const T*>(request);
}

TEST_CASE("Request", "[Server][Server.Request]")
{
    SECTION("Parsing", "[Server][Server.Request][Server.Request.Decoder]")
    {
        REQUIRE(decode<Request>({}) == nullptr);
        REQUIRE(decode<Request>({ 0xff }) == nullptr);

        SECTION("Parsing RegisterUser",
                "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.RegisterUser]")
        {
            using T = RegisterUserRequest;

            REQUIRE(decode<T>({ SEQN_NUMBER, BYTE_FOR(RegisterUser) }) == nullptr);
            REQUIRE(decode<T>({ SEQN_NUMBER, BYTE_FOR(RegisterUser), 0x00 }) == nullptr);
            REQUIRE(decode<T>({ SEQN_NUMBER, BYTE_FOR(RegisterUser), 0x01 }) == nullptr);
            REQUIRE(decode<T>({ SEQN_NUMBER, BYTE_FOR(RegisterUser), 0x01, 'a', 0x00 }) == nullptr);
            REQUIRE(decode<T>({ SEQN_NUMBER, BYTE_FOR(RegisterUser), 0x01, 'a', 0x01 }) == nullptr);

            const auto* a_a = decode<T>({ SEQN_NUMBER, BYTE_FOR(RegisterUser), 0x01, 'a', 0x01, 'a' });
            REQUIRE(a_a != nullptr);
            REQUIRE(a_a->m_type == Request::Type::RegisterUser);
            REQUIRE(a_a->m_name == "a");
            REQUIRE(a_a->m_secret == "a");
            delete a_a;

            const auto* hello_world = decode<T>(
                { SEQN_NUMBER, BYTE_FOR(RegisterUser), 0x05, 'h', 'e', 'l', 'l', 'o', 0x05, 'w', 'o', 'r', 'l', 'd' });
            REQUIRE(hello_world != nullptr);
            REQUIRE(hello_world->m_type == Request::Type::RegisterUser);
            REQUIRE(hello_world->m_name == "hello");
            REQUIRE(hello_world->m_secret == "world");
            delete hello_world;
        }

        SECTION("Parsing NewGame", "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.NewGame]")
        {
            using T = NewGameRequest;
            using IterationType = k10engine::Scenario::IterationType;

            REQUIRE(decode<T>({ SEQN_NUMBER, BYTE_FOR(NewGame) }) == nullptr);
            REQUIRE(decode<T>({ SEQN_NUMBER,
                                BYTE_FOR(NewGame),
                                // PlayerId
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                // PlayerSecret
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                // Scenario (name)
                                0x00,
                                // Parameters: development_card_iteration_type
                                0x00,
                                // Parameters: port_iteration_type
                                0x00,
                                // Parameters: resource_iteration_type
                                0x00,
                                // Parameters: roll_iteration_type
                                0x00,
                                // Parameters: player_count
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                // Parameters: victory_points_goal
                                0x00,
                                0x00,
                                0x00,
                                // 0x00, // missing one byte :O
                                0x00,
                                0x00,
                                0x00,
                                0x00 })
                    == nullptr);

            const auto* all_zeroes = decode<T>({ SEQN_NUMBER,
                                                 BYTE_FOR(NewGame),
                                                 // PlayerId
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 // PlayerSecret
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 // Scenario (name)
                                                 0x00,
                                                 // Parameters: development_card_iteration_type
                                                 0x00,
                                                 // Parameters: port_iteration_type
                                                 0x00,
                                                 // Parameters: resource_iteration_type
                                                 0x00,
                                                 // Parameters: roll_iteration_type
                                                 0x00,
                                                 // Parameters: player_count
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 // Parameters: victory_points_goal
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00 });
            REQUIRE(all_zeroes != nullptr);
            REQUIRE(all_zeroes->m_type == Request::Type::NewGame);
            REQUIRE(all_zeroes->m_player_id == 0);
            REQUIRE(all_zeroes->m_player_secret == 0);
            REQUIRE(all_zeroes->m_scenario == Scenario::Store::the().by_name(static_cast<Scenario::Name>(0)));
            REQUIRE(all_zeroes->m_parameters.development_card_iteration_type == IterationType::Fixed);
            REQUIRE(all_zeroes->m_parameters.port_iteration_type == IterationType::Fixed);
            REQUIRE(all_zeroes->m_parameters.resource_iteration_type == IterationType::Fixed);
            REQUIRE(all_zeroes->m_parameters.roll_iteration_type == IterationType::Fixed);
            REQUIRE(all_zeroes->m_parameters.players_count == 0);
            REQUIRE(all_zeroes->m_parameters.victory_points_goal == 0);
            delete all_zeroes;
        }

        SECTION("Parsing JoinGame", "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.JoinGame]")
        {
            using T = JoinGameRequest;

            REQUIRE(decode<T>({ SEQN_NUMBER, BYTE_FOR(JoinGame) }) == nullptr);
            REQUIRE(decode<T>({ SEQN_NUMBER, BYTE_FOR(JoinGame), 0x00 }) == nullptr);
            REQUIRE(decode<T>({ SEQN_NUMBER, BYTE_FOR(JoinGame), // 15 bytes!
                                0x00,        0x00,
                                0x00,        0x00,
                                0x00,        0x00,
                                0x00,        0x00,
                                0x00,        0x00,
                                0x00,        0x00,
                                0x00,        0x00,
                                0x00,        0x00,
                                0x00,        0x00,
                                0x00,        0x00,
                                0x00,        0x00,
                                0x00 })
                    == nullptr);

            const auto* all_zeroes = decode<T>({ SEQN_NUMBER, BYTE_FOR(JoinGame),
                                                 0x00,        0x00,
                                                 0x00,        0x00,
                                                 0x00,        0x00,
                                                 0x00,        0x00,
                                                 0x00,        0x00,
                                                 0x00,        0x00,
                                                 0x00,        0x00,
                                                 0x00,        0x00,
                                                 0x00,        0x00,
                                                 0x00,        0x00,
                                                 0x00,        0x00,
                                                 0x00,        0x00 });
            REQUIRE(all_zeroes != nullptr);
            REQUIRE(all_zeroes->m_type == Request::Type::JoinGame);
            REQUIRE(all_zeroes->m_player_id == 0);
            REQUIRE(all_zeroes->m_player_secret == 0);
            REQUIRE(all_zeroes->m_game_id == 0);
            delete all_zeroes;

            const auto* deadbeef = decode<T>({ SEQN_NUMBER, BYTE_FOR(JoinGame),
                                               0xde,        0xad,
                                               0xbe,        0xef,
                                               0x00,        0x00,
                                               0x00,        0x00,
                                               0xde,        0xad,
                                               0xbe,        0xef,
                                               0x11,        0x11,
                                               0x11,        0x11,
                                               0xde,        0xad,
                                               0xbe,        0xef,
                                               0x22,        0x22,
                                               0x22,        0x22 });
            REQUIRE(deadbeef != nullptr);
            REQUIRE(deadbeef->m_type == Request::Type::JoinGame);
            REQUIRE(deadbeef->m_player_id == 0xdeadbeef00000000);
            REQUIRE(deadbeef->m_player_secret == 0xdeadbeef11111111);
            REQUIRE(deadbeef->m_game_id == 0xdeadbeef22222222);
            delete deadbeef;
        }

        SECTION("Parsing LeaveGame",
                "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.LeaveGame]")
        {
            // NB: This is identical to JoinGame
            using T = LeaveGameRequest;

            const auto* deadbeef = decode<T>({ SEQN_NUMBER, BYTE_FOR(LeaveGame),
                                               0xde,        0xad,
                                               0xbe,        0xef,
                                               0x00,        0x00,
                                               0x00,        0x00,
                                               0xde,        0xad,
                                               0xbe,        0xef,
                                               0x11,        0x11,
                                               0x11,        0x11,
                                               0xde,        0xad,
                                               0xbe,        0xef,
                                               0x22,        0x22,
                                               0x22,        0x22 });
            REQUIRE(deadbeef != nullptr);
            REQUIRE(deadbeef->m_type == Request::Type::LeaveGame);
            REQUIRE(deadbeef->m_player_id == 0xdeadbeef00000000);
            REQUIRE(deadbeef->m_player_secret == 0xdeadbeef11111111);
            REQUIRE(deadbeef->m_game_id == 0xdeadbeef22222222);
            delete deadbeef;
        }

        SECTION("Parsing StartGame",
                "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.StartGame]")
        {
            // NB: This is identical to JoinGame
            using T = StartGameRequest;

            const auto* deadbeef = decode<T>({ SEQN_NUMBER, BYTE_FOR(StartGame),
                                               0xde,        0xad,
                                               0xbe,        0xef,
                                               0x00,        0x00,
                                               0x00,        0x00,
                                               0xde,        0xad,
                                               0xbe,        0xef,
                                               0x11,        0x11,
                                               0x11,        0x11,
                                               0xde,        0xad,
                                               0xbe,        0xef,
                                               0x22,        0x22,
                                               0x22,        0x22 });
            REQUIRE(deadbeef != nullptr);
            REQUIRE(deadbeef->m_type == Request::Type::StartGame);
            REQUIRE(deadbeef->m_player_id == 0xdeadbeef00000000);
            REQUIRE(deadbeef->m_player_secret == 0xdeadbeef11111111);
            REQUIRE(deadbeef->m_game_id == 0xdeadbeef22222222);
            delete deadbeef;
        }

        SECTION("Parsing MakeMove", "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.MakeMove]")
        {
            using T = MakeMoveRequest;

            const auto* build_settlement = decode<T>({ SEQN_NUMBER, BYTE_FOR(MakeMove),
                                                       0xde,        0xad,
                                                       0xbe,        0xef,
                                                       0x00,        0x00,
                                                       0x00,        0x00,
                                                       0xde,        0xad,
                                                       0xbe,        0xef,
                                                       0x11,        0x11,
                                                       0x11,        0x11,
                                                       0xde,        0xad,
                                                       0xbe,        0xef,
                                                       0x22,        0x22,
                                                       0x22,        0x22,
                                                       0x04,        0x00,
                                                       0x00,        0x00,
                                                       0x00,        0x00,
                                                       0x00,        0x00,
                                                       0x01,        0x00,
                                                       0x00,        0x00,
                                                       0x00,        0x00,
                                                       0x00,        0x00,
                                                       0x00,        0x2a });
            REQUIRE(build_settlement != nullptr);
            REQUIRE(build_settlement->m_type == Request::Type::MakeMove);
            REQUIRE(build_settlement->m_player_id == 0xdeadbeef00000000);
            REQUIRE(build_settlement->m_player_secret == 0xdeadbeef11111111);
            REQUIRE(build_settlement->m_game_id == 0xdeadbeef22222222);

            REQUIRE(build_settlement->m_action.edge == State::Edge::BuildSettlement);
            REQUIRE(build_settlement->m_action.args.size() == 1);
            REQUIRE(build_settlement->m_action.args.at(0).type == Game::ActionArgumentType::NodeId);
            REQUIRE(build_settlement->m_action.args.at(0).value == 42);
            delete build_settlement;
        }

        SECTION("Parsing Query", "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.Query]")
        {
            // NB: This is identical to JoinGame
            using T = QueryRequest;

            const auto* deadbeef = decode<T>({ SEQN_NUMBER, BYTE_FOR(Query),
                                               0xde,        0xad,
                                               0xbe,        0xef,
                                               0x00,        0x00,
                                               0x00,        0x00,
                                               0xde,        0xad,
                                               0xbe,        0xef,
                                               0x11,        0x11,
                                               0x11,        0x11,
                                               0xde,        0xad,
                                               0xbe,        0xef,
                                               0x22,        0x22,
                                               0x22,        0x22 });
            REQUIRE(deadbeef != nullptr);
            REQUIRE(deadbeef->m_type == Request::Type::Query);
            REQUIRE(deadbeef->m_player_id == 0xdeadbeef00000000);
            REQUIRE(deadbeef->m_player_secret == 0xdeadbeef11111111);
            REQUIRE(deadbeef->m_game_id == 0xdeadbeef22222222);
            delete deadbeef;
        }

        SECTION("Parsing RegisterListener",
                "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.RegisterListener]")
        {
            using T = RegisterListenerRequest;
            // FIXME: Implement!
        }

        SECTION("Parsing UnregisterListener",
                "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.UnregisterListener]")
        {
            using T = UnregisterListenerRequest;
            // FIXME: Implement!
        }
    }
}

} // namespace k10engine::Server
