#include <ADT/Assertions.h>
#include <ADT/SinglyLinkedList.h>
#include <ADT/DoublyLinkedList.h>
#include <ADT/HashTable.h>

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
	ASSERT(! l.is_empty());
	ASSERT(l.first() == 69);
	ASSERT(l.last() == 69);
	ASSERT(l.contains_slow(69));
	ASSERT(! l.contains_slow(70));

	l.append(70);

	ASSERT(l.size_slow() == 2);
	ASSERT(! l.is_empty());
	ASSERT(l.first() == 69);
	ASSERT(l.last() == 70);
	ASSERT(l.contains_slow(69));
	ASSERT(l.contains_slow(70));
	ASSERT(! l.contains_slow(71));

	int v = l.take_first();

	ASSERT(v == 69);
	ASSERT(l.size_slow() == 1);
	ASSERT(! l.is_empty());
	ASSERT(l.first() == 70);
	ASSERT(l.last() == 70);
	ASSERT(! l.contains_slow(69));
	ASSERT(l.contains_slow(70));
	ASSERT(! l.contains_slow(71));

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
	ASSERT(! l.contains_slow(69));
	ASSERT(l.contains_slow(70));
	ASSERT(l.contains_slow(71));
	ASSERT(! l.contains_slow(72));
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
	ASSERT(! l.is_empty());
	ASSERT(l.first() == 69);
	ASSERT(l.last() == 69);
	ASSERT(l.contains_slow(69));
	ASSERT(! l.contains_slow(70));

	l.append(70);

	ASSERT(l.size_slow() == 2);
	ASSERT(! l.is_empty());
	ASSERT(l.first() == 69);
	ASSERT(l.last() == 70);
	ASSERT(l.contains_slow(69));
	ASSERT(l.contains_slow(70));
	ASSERT(! l.contains_slow(71));

	l.prepend(68);

	ASSERT(l.size_slow() == 3);
	ASSERT(! l.is_empty());
	ASSERT(l.first() == 68);
	ASSERT(l.last() == 70);
	ASSERT(l.contains_slow(68));
	ASSERT(l.contains_slow(69));
	ASSERT(l.contains_slow(70));
	ASSERT(! l.contains_slow(71));

	int v;
	v = l.take_first();

	ASSERT(v == 68);
	ASSERT(l.size_slow() == 2);
	ASSERT(! l.is_empty());
	ASSERT(l.first() == 69);
	ASSERT(l.last() == 70);
	ASSERT(! l.contains_slow(68));
	ASSERT(l.contains_slow(69));
	ASSERT(l.contains_slow(70));
	ASSERT(! l.contains_slow(71));

	v = l.take_last(); 

	ASSERT(v == 70);
	ASSERT(l.size_slow() == 1);
	ASSERT(! l.is_empty());
	ASSERT(l.first() == 69);
	ASSERT(l.last() == 69);
	ASSERT(! l.contains_slow(68));
	ASSERT(l.contains_slow(69));
	ASSERT(! l.contains_slow(70));
	ASSERT(! l.contains_slow(71));

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
	ASSERT(! l.contains_slow(68));
	ASSERT(l.contains_slow(69));
	ASSERT(l.contains_slow(70));
	ASSERT(l.contains_slow(71));
	ASSERT(! l.contains_slow(72));

	l.clear();

	ASSERT(l.size_slow() == 0);
	ASSERT(l.is_empty());
}

/*
void run_inline_linked_list_tests()
{
	ADT::InlineLinkedList<ADT::InlineLinkedListNode<int>> l;

	ASSERT(l.size_slow() == 0);
	ASSERT(l.is_empty());

	l.clear();

	ASSERT(l.size_slow() == 0);
	ASSERT(l.is_empty());

	int v68 = ADT::InlineLinkedListNode<int>(68);
	int v69 = ADT::InlineLinkedListNode<int>(69);
	int v70 = ADT::InlineLinkedListNode<int>(70);
	int v71 = ADT::InlineLinkedListNode<int>(71);
	int v72 = ADT::InlineLinkedListNode<int>(72);
	int v80 = ADT::InlineLinkedListNode<int>(80);
	int v81 = ADT::InlineLinkedListNode<int>(81);
	int v82 = ADT::InlineLinkedListNode<int>(82);

	l.append(&v69);

	ASSERT(l.size_slow() == 1);
	ASSERT(! l.is_empty());
	ASSERT(*l.head() == v69);
	ASSERT(l.contains_slow(&v69));
	ASSERT(! l.contains_slow(&v70));

	l.append(&v70);

	ASSERT(l.size_slow() == 2);
	ASSERT(! l.is_empty());
	ASSERT(*l.head() == v69);
	ASSERT(l.contains_slow(&v69));
	ASSERT(l.contains_slow(&v70));
	ASSERT(! l.contains_slow(&v71));

	l.prepend(&v68);

	ASSERT(l.size_slow() == 3);
	ASSERT(! l.is_empty());
	ASSERT(*l.head() == v68);
	ASSERT(l.contains_slow(&v68));
	ASSERT(l.contains_slow(&v69));
	ASSERT(l.contains_slow(&v70));
	ASSERT(! l.contains_slow(&v71));

	int *v;
	v = l.remove_head();

	ASSERT(v == &v68);
	ASSERT(l.size_slow() == 2);
	ASSERT(! l.is_empty());
	ASSERT(*l.head() == v69);
	ASSERT(! l.contains_slow(&v68));
	ASSERT(l.contains_slow(&v69));
	ASSERT(l.contains_slow(&v70));
	ASSERT(! l.contains_slow(&v71));

	v = l.remove_tail(); 

	ASSERT(v == &v70);
	ASSERT(l.size_slow() == 1);
	ASSERT(! l.is_empty());
	ASSERT(*l.head() == v69);
	ASSERT(! l.contains_slow(&v68));
	ASSERT(l.contains_slow(&v69));
	ASSERT(! l.contains_slow(&v70));
	ASSERT(! l.contains_slow(&v71));

	l.append(&v70);
	l.append(&v71);
	l.append(&v72);

	ASSERT(l.size_slow() == 4);

	l.remove(&v72);

	ASSERT(l.size_slow() == 3);
	ASSERT(! l.contains_slow(&v68));
	ASSERT(l.contains_slow(&v69));
	ASSERT(l.contains_slow(&v70));
	ASSERT(l.contains_slow(&v71));
	ASSERT(! l.contains_slow(&v72));

	ADT::InlineLinkedList<int> l2;
	l2.append(&v80);
	l2.append(&v81);
	l2.append(&v82);

	l.concat(l2);

	ASSERT(l.size_slow() == 6);
	ASSERT(l.contains_slow(&v69));
	ASSERT(l.contains_slow(&v70));
	ASSERT(l.contains_slow(&v71));
	ASSERT(l.contains_slow(&v80));
	ASSERT(l.contains_slow(&v81));
	ASSERT(l.contains_slow(&v82));

	l.clear();

	ASSERT(l.size_slow() == 0);
	ASSERT(l.is_empty());
}
*/

void run_hash_table_tests()
{

}

int main(int argc, char *argv[])
{
	run_singly_linked_list_tests();
	run_doubly_linked_list_tests();
	run_hash_table_tests();
	printf("all tests passed\n");
	return 0;
}