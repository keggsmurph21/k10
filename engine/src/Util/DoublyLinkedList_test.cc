#include <cstring>

#include "Test/catch.h"
#include "Util/DoublyLinkedList.h"

template<typename T>
using DLL = DoublyLinkedList<T>;

TEST_CASE("DoublyLinkedList", "[Util][Util.DoublyLinkedList]")
{
    SECTION("constructor & destructor")
    {
        DLL<int> int_list;
        DLL<std::string> str_list;
        DLL<int*> int_ptr_list;
    }

    SECTION("is_empty()")
    {
        DLL<int> int_list;
        DLL<std::string> str_list;
        DLL<int*> int_ptr_list;
        REQUIRE(int_list.is_empty());
        REQUIRE(str_list.is_empty());
        REQUIRE(int_ptr_list.is_empty());
    }

    SECTION("prepend()")
    {
        DLL<int> list;
        list.prepend(0);
        list.prepend(2);
        list.prepend(4);

        REQUIRE(!list.is_empty());
        REQUIRE(list.head() == 4);
        REQUIRE(list.tail() == 0);
    }

    SECTION("append()")
    {
        DLL<int> list;
        list.append(4);
        list.append(2);
        list.append(0);

        REQUIRE(!list.is_empty());
        REQUIRE(list.head() == 4);
        REQUIRE(list.tail() == 0);
    }

    SECTION("clear()")
    {
        DLL<int> list;
        list.append(4);
        list.append(2);
        list.append(0);

        list.clear();

        REQUIRE(list.is_empty());
    }

    SECTION("find()")
    {
        DLL<int> list;
        list.append(4);
        list.append(2);
        list.append(0);

        REQUIRE(*list.find(4) == 4);
        REQUIRE(*list.find(2) == 2);
        REQUIRE(*list.find(0) == 0);
        REQUIRE(list.find(1) == list.end());
    }

    SECTION("contains()")
    {
        DLL<int> list;
        list.append(4);
        list.append(2);
        list.append(0);

        REQUIRE(list.contains(4));
        REQUIRE(list.contains(2));
        REQUIRE(list.contains(0));
        REQUIRE(!list.contains(1));
    }

    SECTION("range-for")
    {
        DLL<int> list;
        list.append(4);
        list.append(2);
        list.append(0);

        std::vector<int> expected{ 4, 2, 0 };
        int index = 0;

        for (auto& value : list) {
            REQUIRE(value == expected.at(index));
            ++index;
        }

        index = 0;

        for (const auto& value : list) {
            REQUIRE(value == expected.at(index));
            ++index;
        }
    }

    SECTION("remove()")
    {
        DLL<int> list;
        list.append(4);
        list.append(2);
        list.append(0);

        REQUIRE(list.head() == 4);
        REQUIRE(list.tail() == 0);
        REQUIRE(list.contains(4));
        REQUIRE(list.contains(2));
        REQUIRE(list.contains(0));

        list.remove(list.find(4));

        REQUIRE(list.head() == 2);
        REQUIRE(list.tail() == 0);
        REQUIRE(!list.contains(4));
        REQUIRE(list.contains(2));
        REQUIRE(list.contains(0));

        list.remove(list.find(0));

        REQUIRE(list.head() == 2);
        REQUIRE(list.tail() == 2);
        REQUIRE(!list.contains(4));
        REQUIRE(list.contains(2));
        REQUIRE(!list.contains(0));

        list.remove(list.find(2));

        REQUIRE(list.is_empty());
    }

    SECTION("pop()")
    {
        DLL<int> list;
        list.append(4);
        list.append(2);
        list.append(0);

        REQUIRE(list.tail() == 0);
        REQUIRE(list.contains(4));
        REQUIRE(list.contains(2));
        REQUIRE(list.contains(0));

        REQUIRE(list.pop() == 0);

        REQUIRE(list.tail() == 2);
        REQUIRE(list.contains(4));
        REQUIRE(list.contains(2));
        REQUIRE(!list.contains(0));

        REQUIRE(list.pop() == 2);

        REQUIRE(list.tail() == 4);
        REQUIRE(list.contains(4));
        REQUIRE(!list.contains(2));
        REQUIRE(!list.contains(0));

        REQUIRE(list.pop() == 4);

        REQUIRE(list.is_empty());
        REQUIRE(!list.contains(4));
        REQUIRE(!list.contains(2));
        REQUIRE(!list.contains(0));
    }

    SECTION("insert_after()")
    {
        // FIXME: Implement this!
    }

    SECTION("DoublyLinkedList<Point*>")
    {
        struct Point {
            int x;
            int y;
            bool operator==(const Point& other) const { return other.x == x && other.y == y; }
        };

        auto p0 = Point{ 0, 0 };
        auto p1 = Point{ 0, 1 };
        auto p2 = Point{ 6, 9 };

        DLL<Point*> list;
        list.append(&p0);
        list.append(&p1);
        list.append(&p2);

        REQUIRE(*list.pop() == p2);
        REQUIRE(*list.head() == p0);
        REQUIRE(*list.tail() == p1);
    }
}
