#include <ADT/Assertions.h>
#include <ADT/DoublyLinkedList.h>
#include <ADT/HashTable.h>
#include <ADT/SinglyLinkedList.h>

void run_singly_linked_list_tests()
{
    ADT::SinglyLinkedList<int> l;

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
    ADT::DoublyLinkedList<int> l;

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

    l.clear();

    ASSERT(l.size_slow() == 0);
    ASSERT(l.is_empty());
}

void run_hash_table_tests() {}

int main(int, char**)
{
    run_singly_linked_list_tests();
    run_doubly_linked_list_tests();
    run_hash_table_tests();
    printf("all tests passed\n");
    return 0;
}
