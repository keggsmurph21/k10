#include <cstring>
#include <iostream>
#include <random>

#include "Test/catch.h"
#include "Util/LinkedTrie.h"
#include "Util/Types.h"

static const u8 KEY_LEN = 4;

TEST_CASE("LinkedTrie", "[Util][Util.LinkedTrie]")
{
    SECTION("u8* -> u8")
    {
        static u8 a0000000[] = { 0xa0, 0x00, 0x00, 0x00 };
        static u8 a0000001[] = { 0xa0, 0x00, 0x00, 0x01 };
        static u8 a0000002[] = { 0xa0, 0x00, 0x00, 0x02 };
        static u8 a0000003[] = { 0xa0, 0x00, 0x00, 0x03 };
        static u8 a0000004[] = { 0xa0, 0x00, 0x00, 0x04 };

        LinkedTrie<u8, u8> trie;

        const auto compare = [&](u8* key, const u8* expected_value) -> bool {
            const auto* actual_value = trie.get(key, KEY_LEN);
            if (actual_value == nullptr || expected_value == nullptr)
                return actual_value == nullptr && expected_value == nullptr;
            return *actual_value == *expected_value;
        };

        REQUIRE(compare(a0000000, nullptr));
        REQUIRE(compare(a0000001, nullptr));
        REQUIRE(compare(a0000002, nullptr));
        REQUIRE(compare(a0000003, nullptr));
        REQUIRE(compare(a0000004, nullptr));

        trie.set(a0000002, KEY_LEN, *a0000002);

        REQUIRE(compare(a0000000, nullptr));
        REQUIRE(compare(a0000001, nullptr));
        REQUIRE(compare(a0000002, a0000002));
        REQUIRE(compare(a0000003, nullptr));
        REQUIRE(compare(a0000004, nullptr));

        trie.set(a0000000, KEY_LEN, *a0000000);

        REQUIRE(compare(a0000000, a0000000));
        REQUIRE(compare(a0000001, nullptr));
        REQUIRE(compare(a0000002, a0000002));
        REQUIRE(compare(a0000003, nullptr));
        REQUIRE(compare(a0000004, nullptr));

        trie.set(a0000001, KEY_LEN, *a0000001);

        REQUIRE(compare(a0000000, a0000000));
        REQUIRE(compare(a0000001, a0000001));
        REQUIRE(compare(a0000002, a0000002));
        REQUIRE(compare(a0000003, nullptr));
        REQUIRE(compare(a0000004, nullptr));

        trie.set(a0000004, KEY_LEN, *a0000004);

        REQUIRE(compare(a0000000, a0000000));
        REQUIRE(compare(a0000001, a0000001));
        REQUIRE(compare(a0000002, a0000002));
        REQUIRE(compare(a0000003, nullptr));
        REQUIRE(compare(a0000004, a0000004));

        trie.set(a0000003, KEY_LEN, *a0000003);

        REQUIRE(compare(a0000000, a0000000));
        REQUIRE(compare(a0000001, a0000001));
        REQUIRE(compare(a0000002, a0000002));
        REQUIRE(compare(a0000003, a0000003));
        REQUIRE(compare(a0000004, a0000004));
    }

    SECTION("u8 -> u8*")
    {
        LinkedTrie<u8, u8*> trie;

        u8 v1 = 0x69;
        u8 v2 = 0x42;
        u8 v3 = 0x00;

        trie.set(&v1, 1, &v1);
        trie.set(&v2, 1, nullptr);

        REQUIRE(*(*trie.get(&v1, 1)) == v1);
        REQUIRE(*trie.get(&v2, 1) == nullptr);
        REQUIRE(trie.get(&v3, 1) == nullptr);

        REQUIRE(trie.contains(&v1, 1));
        REQUIRE(trie.contains(&v2, 1));
        REQUIRE(!trie.contains(&v3, 1));
    }

    SECTION("u8* -> { u8*, u8, size_t, size_t }")
    {
        struct Data {
        public:
            bool operator==(const Data& other) const
            {
                if (secret_len != other.secret_len)
                    return false;
                for (size_t i = 0; i < secret_len; ++i) {
                    const auto* my_secret = this->secret;
                    const auto* other_secret = other.secret;
                    if (my_secret != other_secret)
                        return false;
                }
                return counter == other.counter;
            }

            u8* secret;     // NOLINT(misc-non-private-member-variables-in-classes)
            u8 secret_len;  // NOLINT(misc-non-private-member-variables-in-classes)
            size_t counter; // NOLINT(misc-non-private-member-variables-in-classes)
        };

        static size_t s_counter = 0;

        auto make_data = [&](u8* secret) -> Data {
            Data data;
            data.secret = secret;
            data.secret_len = KEY_LEN;
            data.counter = ++s_counter;
            return data;
        };

        static u8 d00dfade[] = { 0xd0, 0x0d, 0xfa, 0xde };
        static u8 deadc0de[] = { 0xde, 0xad, 0xc0, 0xde };
        static u8 deadbeef[] = { 0xde, 0xad, 0xbe, 0xef };
        static u8 deadbfff[] = { 0xde, 0xad, 0xbf, 0xff };

        static auto d00dfade_data = make_data(d00dfade);
        static auto deadc0de_data = make_data(deadc0de);
        static auto deadbeef_data = make_data(deadbeef);
        static auto deadbfff_data = make_data(deadbfff);

        LinkedTrie<u8, Data> trie;

        const auto compare = [&](u8* key, const Data* expected_value) -> bool {
            const auto* actual_value = trie.get(key, KEY_LEN);
            if (actual_value == nullptr || expected_value == nullptr)
                return actual_value == nullptr && expected_value == nullptr;
            return *actual_value == *expected_value;
        };

        REQUIRE(compare(d00dfade, nullptr));
        REQUIRE(compare(deadc0de, nullptr));
        REQUIRE(compare(deadbeef, nullptr));
        REQUIRE(compare(deadbfff, nullptr));

        trie.set(d00dfade, KEY_LEN, d00dfade_data);

        REQUIRE(compare(d00dfade, &d00dfade_data));
        REQUIRE(compare(deadc0de, nullptr));
        REQUIRE(compare(deadbeef, nullptr));
        REQUIRE(compare(deadbfff, nullptr));

        trie.set(deadc0de, KEY_LEN, deadc0de_data);

        REQUIRE(compare(d00dfade, &d00dfade_data));
        REQUIRE(compare(deadc0de, &deadc0de_data));
        REQUIRE(compare(deadbeef, nullptr));
        REQUIRE(compare(deadbfff, nullptr));

        trie.set(deadbeef, KEY_LEN, deadbeef_data);

        REQUIRE(compare(d00dfade, &d00dfade_data));
        REQUIRE(compare(deadc0de, &deadc0de_data));
        REQUIRE(compare(deadbeef, &deadbeef_data));
        REQUIRE(compare(deadbfff, nullptr));

        trie.set(deadbfff, KEY_LEN, deadbfff_data);

        REQUIRE(compare(d00dfade, &d00dfade_data));
        REQUIRE(compare(deadc0de, &deadc0de_data));
        REQUIRE(compare(deadbeef, &deadbeef_data));
        REQUIRE(compare(deadbfff, &deadbfff_data));

        REQUIRE(!compare(d00dfade, &deadc0de_data));
        REQUIRE(!compare(d00dfade, &deadbeef_data));
        REQUIRE(!compare(d00dfade, &deadbfff_data));
    }

    SECTION("char* -> int")
    {
        LinkedTrie<const char, int> trie;

        const auto compare = [&](const char* key, const int* expected_value) -> bool {
            const auto* actual_value = trie.get(key, strlen(key));
            if (actual_value == nullptr || expected_value == nullptr)
                return actual_value == nullptr && expected_value == nullptr;
            return *actual_value == *expected_value;
        };

        const char* abc = "abc";
        int abc_data = 3;

        const char* abcd = "abcd";
        int abcd_data = 4;

        const char* abcde = "abcde";
        int abcde_data = 5;

        REQUIRE(compare(abc, nullptr));
        REQUIRE(compare(abcd, nullptr));
        REQUIRE(compare(abcde, nullptr));

        trie.set(abcd, 4, abcd_data);

        REQUIRE(compare(abc, nullptr));
        REQUIRE(compare(abcd, &abcd_data));
        REQUIRE(compare(abcde, nullptr));

        trie.set(abcde, 5, abcde_data);

        REQUIRE(compare(abc, nullptr));
        REQUIRE(compare(abcd, &abcd_data));
        REQUIRE(compare(abcde, &abcde_data));

        trie.set(abc, 3, abc_data);

        REQUIRE(compare(abc, &abc_data));
        REQUIRE(compare(abcd, &abcd_data));
        REQUIRE(compare(abcde, &abcde_data));
    }
}
