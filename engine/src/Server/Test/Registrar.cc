#include <cstring>

#include "Server/Registrar.h"
#include "Test/catch.h"

TEST_CASE("Registrar", "[Server][Server.Registrar]")
{
    k10engine::Server::Registrar registrar;

    REQUIRE(!registrar.validate_player(0, 0));

    const char* username = "test-name";
    const char* secret = "test-secret";
    const auto registration = registrar.register_user(username, strlen(username), secret, strlen(secret));

    REQUIRE(registration.has_value());
    REQUIRE(registrar.validate_player(registration->player_id, registration->internal_secret));
    REQUIRE(!registrar.validate_player(registration->player_id, 1 + registration->internal_secret));

    const auto re_registration = registrar.register_user(username, strlen(username), secret, strlen(secret));

    REQUIRE(re_registration.has_value());
    REQUIRE(re_registration->player_id == registration->player_id);
    REQUIRE(re_registration->internal_secret == registration->internal_secret);

    REQUIRE(!registrar.register_user(username, strlen(username), "wrong", strlen("wrong")).has_value());
}
