#include <ADT/Assertions.h>
#include <ADT/DoublyLinkedList.h>
#include <ADT/Extras.h>
#include <ADT/HashMap.h>
#include <ADT/HashTable.h>
#include <ADT/Point.h>
#include <ADT/SinglyLinkedList.h>
#include <stdio.h>

using namespace ADT;

void run_singly_linked_list_tests()
{
    printf("... running SinglyLinkedList tests ...\n");
    SinglyLinkedList<int> l;

    ASSERT(l.size_slow() == 0);
    ASSERT(l.is_empty());

    l.clear();

    ASSERT(l.size_slow() == 0);
    ASSERT(l.is_empty());

    l.append(69);

    ASSERT(l.size_slow() == 1);
    ASSERT(!l.is_empty());
    ASSERT(l.first() == 69);
    ASSERT(l.last() == 69);
    ASSERT(l.contains_slow(69));
    ASSERT(!l.contains_slow(70));

    l.append(70);

    ASSERT(l.size_slow() == 2);
    ASSERT(!l.is_empty());
    ASSERT(l.first() == 69);
    ASSERT(l.last() == 70);
    ASSERT(l.contains_slow(69));
    ASSERT(l.contains_slow(70));
    ASSERT(!l.contains_slow(71));

    int v = l.take_first();

    ASSERT(v == 69);
    ASSERT(l.size_slow() == 1);
    ASSERT(!l.is_empty());
    ASSERT(l.first() == 70);
    ASSERT(l.last() == 70);
    ASSERT(!l.contains_slow(69));
    ASSERT(l.contains_slow(70));
    ASSERT(!l.contains_slow(71));

    ASSERT(l.find(69).is_end());
    ASSERT(*l.find(70) == 70);
    ASSERT(l.find(71).is_end());

    l.append(71);
    l.append(72);
    l.append(73);

    ASSERT(l.size_slow() == 4);
    ASSERT(l.find(71) == l.find(71));
    ASSERT(l.find(71) != l.find(72));

    l.remove(l.find(72));

    ASSERT(l.size_slow() == 3);
    ASSERT(!l.contains_slow(69));
    ASSERT(l.contains_slow(70));
    ASSERT(l.contains_slow(71));
    ASSERT(!l.contains_slow(72));
    ASSERT(l.contains_slow(73));

    l.clear();

    ASSERT(l.size_slow() == 0);
    ASSERT(l.is_empty());
}

void run_doubly_linked_list_tests()
{
    printf("... running DoublyLinkedList tests ...\n");
    DoublyLinkedList<int> l;

    ASSERT(l.size_slow() == 0);
    ASSERT(l.is_empty());

    l.clear();

    ASSERT(l.size_slow() == 0);
    ASSERT(l.is_empty());

    l.append(69);

    ASSERT(l.size_slow() == 1);
    ASSERT(!l.is_empty());
    ASSERT(l.first() == 69);
    ASSERT(l.last() == 69);
    ASSERT(l.contains_slow(69));
    ASSERT(!l.contains_slow(70));

    l.append(70);

    ASSERT(l.size_slow() == 2);
    ASSERT(!l.is_empty());
    ASSERT(l.first() == 69);
    ASSERT(l.last() == 70);
    ASSERT(l.contains_slow(69));
    ASSERT(l.contains_slow(70));
    ASSERT(!l.contains_slow(71));

    l.prepend(68);

    ASSERT(l.size_slow() == 3);
    ASSERT(!l.is_empty());
    ASSERT(l.first() == 68);
    ASSERT(l.last() == 70);
    ASSERT(l.contains_slow(68));
    ASSERT(l.contains_slow(69));
    ASSERT(l.contains_slow(70));
    ASSERT(!l.contains_slow(71));

    int v;
    v = l.take_first();

    ASSERT(v == 68);
    ASSERT(l.size_slow() == 2);
    ASSERT(!l.is_empty());
    ASSERT(l.first() == 69);
    ASSERT(l.last() == 70);
    ASSERT(!l.contains_slow(68));
    ASSERT(l.contains_slow(69));
    ASSERT(l.contains_slow(70));
    ASSERT(!l.contains_slow(71));

    v = l.take_last();

    ASSERT(v == 70);
    ASSERT(l.size_slow() == 1);
    ASSERT(!l.is_empty());
    ASSERT(l.first() == 69);
    ASSERT(l.last() == 69);
    ASSERT(!l.contains_slow(68));
    ASSERT(l.contains_slow(69));
    ASSERT(!l.contains_slow(70));
    ASSERT(!l.contains_slow(71));

    ASSERT(l.find(68).is_end());
    ASSERT(*l.find(69) == 69);
    ASSERT(l.find(70).is_end());

    l.append(70);
    l.append(71);
    l.append(72);

    ASSERT(l.size_slow() == 4);
    ASSERT(l.find(71) == l.find(71));
    ASSERT(l.find(71) != l.find(72));

    l.remove(l.find(72));

    ASSERT(l.size_slow() == 3);
    ASSERT(!l.contains_slow(68));
    ASSERT(l.contains_slow(69));
    ASSERT(l.contains_slow(70));
    ASSERT(l.contains_slow(71));
    ASSERT(!l.contains_slow(72));

    for (auto& it : l) {
        printf("%d\n", it);
    }

    l.clear();

    ASSERT(l.size_slow() == 0);
    ASSERT(l.is_empty());
}

void run_hash_table_tests()
{
    printf("... running HashTable tests ...\n");
    HashTable<bool> t_bool;

    ASSERT(t_bool.is_empty());
    ASSERT(t_bool.size() == 0);
    ASSERT(t_bool.capacity() == 0);
    ASSERT(!t_bool.contains(true));
    ASSERT(!t_bool.contains(false));

    t_bool.set(true);

    ASSERT(!t_bool.is_empty());
    ASSERT(t_bool.size() == 1);
    ASSERT(t_bool.capacity() == 2);
    ASSERT(t_bool.contains(true));
    ASSERT(!t_bool.contains(false));

    t_bool.set(true);

    ASSERT(!t_bool.is_empty());
    ASSERT(t_bool.size() == 1);
    ASSERT(t_bool.capacity() == 2);
    ASSERT(t_bool.contains(true));
    ASSERT(!t_bool.contains(false));

    t_bool.set(false);
    t_bool.dump();
    for (auto& e : t_bool)
        printf("%s\n", e ? "true" : "false");

    ASSERT(!t_bool.is_empty());
    ASSERT(t_bool.size() == 2);
    ASSERT(t_bool.capacity() == 2);
    ASSERT(t_bool.contains(true));
    ASSERT(t_bool.contains(false));

    t_bool.remove(true);

    ASSERT(!t_bool.is_empty());
    ASSERT(t_bool.size() == 1);
    ASSERT(t_bool.capacity() == 2);
    ASSERT(!t_bool.contains(true));
    ASSERT(t_bool.contains(false));

    t_bool.remove(true); // shouldn't throw
    t_bool.clear();

    ASSERT(t_bool.is_empty());
    ASSERT(t_bool.size() == 0);
    ASSERT(t_bool.capacity() == 0);
    ASSERT(!t_bool.contains(true));
    ASSERT(!t_bool.contains(false));

    HashTable<int> t_int;

    ASSERT(t_int.is_empty());
    ASSERT(t_int.size() == 0);
    ASSERT(t_int.capacity() == 0);
    ASSERT(!t_int.contains(68));
    ASSERT(!t_int.contains(69));
    ASSERT(!t_int.contains(70));

    t_int.set(69);

    ASSERT(!t_int.is_empty());
    ASSERT(t_int.size() == 1);
    ASSERT(t_int.capacity() == 2);
    ASSERT(!t_int.contains(68));
    ASSERT(t_int.contains(69));
    ASSERT(!t_int.contains(70));

    t_int.set(68);
    t_int.set(69);
    t_int.set(70);

    ASSERT(!t_int.is_empty());
    ASSERT(t_int.size() == 3);
    ASSERT(t_int.capacity() == 6);
    ASSERT(t_int.contains(68));
    ASSERT(t_int.contains(69));
    ASSERT(t_int.contains(70));

    t_int.set(71);
    t_int.set(72);
    t_int.set(73);
    t_int.set(74);
    t_int.set(75);
    t_int.set(76);

    ASSERT(t_int.size() == 9);
    ASSERT(t_int.capacity() == 14);

    t_int.set(82);
    t_int.set(83);
    t_int.set(84);
    t_int.set(85);
    t_int.set(86);

    ASSERT(t_int.capacity() == 14);

    t_int.set(88);

    ASSERT(t_int.capacity() == 30);

    t_int.dump();

    using Point = Point<int>;
    HashTable<Point> t_point;

    t_point.set(Point());

    ASSERT(!t_point.is_empty());
    ASSERT(t_point.size() == 1);
    ASSERT(t_point.capacity() == 2);
    ASSERT(t_point.contains(Point()));
    ASSERT(t_point.contains(Point(0, 0)));
    ASSERT(!t_point.contains(Point(1, 1)));

    t_point.set(Point(0, 0));

    ASSERT(t_point.size() == 1);

    t_point.set(Point(0, 1));
    t_point.set(Point(1, 0));
    t_point.set(Point(0, -1));
    t_point.set(Point(-1, 0));

    ASSERT(t_point.size() == 5);

    t_point.dump();
}

void run_hash_map_tests()
{
    printf("... running HashMap tests ...\n");
    HashMap<bool, bool> m_bool_bool;

    ASSERT(m_bool_bool.is_empty());
    ASSERT(m_bool_bool.size() == 0);
    ASSERT(m_bool_bool.capacity() == 0);
    ASSERT(!m_bool_bool.contains(true));
    ASSERT(!m_bool_bool.contains(false));
    ASSERT(!(m_bool_bool.get(true)).has_value());
    ASSERT(!(m_bool_bool.get(false)).has_value());

    m_bool_bool.set(true, true);

    ASSERT(!m_bool_bool.is_empty());
    ASSERT(m_bool_bool.size() == 1);
    ASSERT(m_bool_bool.capacity() == 2);
    ASSERT(m_bool_bool.contains(true));
    ASSERT(!m_bool_bool.contains(false));
    ASSERT((m_bool_bool.get(true)).has_value());
    ASSERT((m_bool_bool.get(true)).value() == true);
    ASSERT(!(m_bool_bool.get(false)).has_value());

    m_bool_bool.remove(true);

    ASSERT(m_bool_bool.is_empty());
    ASSERT(m_bool_bool.size() == 0);
    ASSERT(m_bool_bool.capacity() == 2);
    ASSERT(!m_bool_bool.contains(true));
    ASSERT(!(m_bool_bool.get(true)).has_value());

    m_bool_bool.ensure(true);

    ASSERT(!m_bool_bool.is_empty());
    ASSERT(m_bool_bool.size() == 1);
    ASSERT(m_bool_bool.capacity() == 2);
    ASSERT(m_bool_bool.contains(true));
    ASSERT((m_bool_bool.get(true)).has_value());
    ASSERT((m_bool_bool.get(true)).value() == false);

    m_bool_bool.set(true, true);

    ASSERT(m_bool_bool.size() == 1);
    ASSERT((m_bool_bool.get(true)).has_value());
    ASSERT((m_bool_bool.get(true)).value() == true);

    m_bool_bool.dump();

    using Point = Point<int>;
    HashMap<Point, double> m_point_double;

    ASSERT(m_point_double.is_empty());
    ASSERT(m_point_double.size() == 0);
    ASSERT(m_point_double.capacity() == 0);
    ASSERT(!m_point_double.contains(Point()));
    ASSERT(!(m_point_double.get(Point())).has_value());

    m_point_double.set(Point(0, 1), 1.0);
    m_point_double.set(Point(0, 1), 2.0);
    m_point_double.set(Point(6, 9), 4.20);

    ASSERT(m_point_double.size() == 2);
    ASSERT((m_point_double.get(Point(0, 1))).value() == 2.0);
    ASSERT((m_point_double.get(Point(6, 9))).value() == 4.2);

    m_point_double.dump();
}

int main(int, char**)
{
    run_singly_linked_list_tests();
    run_doubly_linked_list_tests();
    run_hash_table_tests();
    run_hash_map_tests();
    printf("... all tests passed!\n");
    return 0;
}
