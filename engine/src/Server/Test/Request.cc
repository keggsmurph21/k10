#include <cassert>
#include <iostream>

#include "Server/Request.h"
#include "Test/catch.h"
#include "Util/ByteBuffer.h"
#include "Util/Decoder.h"
#include "Util/Types.h"

#define BYTE_FOR(x) (static_cast<u8>(Request::Type::x))

namespace k10engine::Server {

template<typename T>
const T* decode(std::vector<u8> bytes)
{
    ByteBuffer buf;
    for (auto byte : bytes)
        buf.append(byte);
    Decoder decoder(buf);
    return static_cast<const T*>(Request::decode(decoder));
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

            REQUIRE(decode<T>({ BYTE_FOR(RegisterUser) }) == nullptr);
            REQUIRE(decode<T>({ BYTE_FOR(RegisterUser), 0x00 }) == nullptr);
            REQUIRE(decode<T>({ BYTE_FOR(RegisterUser), 0x01 }) == nullptr);
            REQUIRE(decode<T>({ BYTE_FOR(RegisterUser), 0x01, 'a', 0x00 }) == nullptr);
            REQUIRE(decode<T>({ BYTE_FOR(RegisterUser), 0x01, 'a', 0x01 }) == nullptr);

            const auto* a_a = decode<T>({ BYTE_FOR(RegisterUser), 0x01, 'a', 0x01, 'a' });
            REQUIRE(a_a != nullptr);
            REQUIRE(a_a->m_type == Request::Type::RegisterUser);
            REQUIRE(a_a->m_name == "a");
            REQUIRE(a_a->m_secret == "a");
            delete a_a;

            const auto* hello_world =
                decode<T>({ BYTE_FOR(RegisterUser), 0x05, 'h', 'e', 'l', 'l', 'o', 0x05, 'w', 'o', 'r', 'l', 'd' });
            REQUIRE(hello_world != nullptr);
            REQUIRE(hello_world->m_type == Request::Type::RegisterUser);
            REQUIRE(hello_world->m_name == "hello");
            REQUIRE(hello_world->m_secret == "world");
            delete hello_world;
        }

        SECTION("Parsing NewGame", "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.NewGame]")
        {
            using T = NewGameRequest;
            // FIXME: Implement!
        }

        SECTION("Parsing JoinGame", "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.JoinGame]")
        {
            using T = JoinGameRequest;
            const auto len = sizeof(Registrar::PlayerId) + sizeof(Registrar::PlayerSecret) + sizeof(GameId);

            REQUIRE(decode<T>({ BYTE_FOR(JoinGame) }) == nullptr);
            REQUIRE(decode<T>({ BYTE_FOR(JoinGame), 0x00 }) == nullptr);
            REQUIRE(decode<T>({ BYTE_FOR(JoinGame), // 15 bytes!
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00 })
                    == nullptr);

            const auto* all_zeroes = decode<T>({ BYTE_FOR(JoinGame),
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00,
                                                 0x00 });
            REQUIRE(all_zeroes != nullptr);
            REQUIRE(all_zeroes->m_type == Request::Type::JoinGame);
            REQUIRE(all_zeroes->m_player_id == 0);
            REQUIRE(all_zeroes->m_player_secret == 0);
            REQUIRE(all_zeroes->m_game_id == 0);
            delete all_zeroes;

            const auto* deadbeef = decode<T>({ BYTE_FOR(JoinGame),
                                               0xde,
                                               0xad,
                                               0xbe,
                                               0xef,
                                               0x00,
                                               0x00,
                                               0x00,
                                               0x00,
                                               0xde,
                                               0xad,
                                               0xbe,
                                               0xef,
                                               0x11,
                                               0x11,
                                               0x11,
                                               0x11,
                                               0xde,
                                               0xad,
                                               0xbe,
                                               0xef,
                                               0x22,
                                               0x22,
                                               0x22,
                                               0x22 });
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
            using T = LeaveGameRequest;
            // FIXME: Implement!
        }

        SECTION("Parsing StartGame",
                "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.StartGame]")
        {
            using T = StartGameRequest;
            // FIXME: Implement!
        }

        SECTION("Parsing MakeMove", "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.MakeMove]")
        {
            using T = MakeMoveRequest;
            // FIXME: Implement!
        }

        SECTION("Parsing Query", "[Server][Server.Request][Server.Request.Decoder][Server.Request.Decoder.Query]")
        {
            using T = QueryRequest;
            // FIXME: Implement!
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
