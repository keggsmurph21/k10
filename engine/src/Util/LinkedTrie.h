#pragma once

#include <cassert>
#include <iostream>
#include <vector>

//#define LINKED_TRIE_DEBUG

template<typename KeyItem, typename Value>
class LinkedTrie {
public:
    LinkedTrie();

    bool contains(const KeyItem*, size_t key_len) const;
    const Value* get(const KeyItem* key, size_t key_len) const;
    void set(const KeyItem* key, size_t key_len, Value);

private:
    struct Node {
        KeyItem key_item;
        size_t index;
        size_t sibling_index;
        size_t child_index;
        size_t value_index;
    };

    inline Node* node_at(size_t index) { return &m_nodes.at(index); }
    inline const Node* node_at(size_t index) const { return &m_nodes.at(index); }

    inline Node* root() { return node_at(0); }
    inline const Node* root() const { return node_at(0); }

    inline Node* sibling_of(const Node& n) { return n.sibling_index ? node_at(n.sibling_index) : nullptr; }
    inline const Node* sibling_of(const Node& n) const { return n.sibling_index ? node_at(n.sibling_index) : nullptr; }

    inline Node* child_of(const Node& n) { return n.child_index ? node_at(n.child_index) : nullptr; }
    inline const Node* child_of(const Node& n) const { return n.child_index ? node_at(n.child_index) : nullptr; }

    inline Value* value_for(const Node& n) { return n.value_index ? &m_values.at(n.value_index) : nullptr; }
    inline const Value* value_for(const Node& n) const { return n.value_index ? &m_values.at(n.value_index) : nullptr; }

    Node& make_node(KeyItem);
    void dump_node(const Node&) const;

    std::vector<Node> m_nodes;
    std::vector<Value> m_values;
};

template<typename KeyItem, typename Value>
LinkedTrie<KeyItem, Value>::LinkedTrie()
{
    // NB: First node.key_item and value are garbage.
    m_nodes.push_back({ { /* garbage */ }, 0, 0, 0, 0 });
    m_values.push_back({ /* garbage */ });
}

template<typename KeyItem, typename Value>
void LinkedTrie<KeyItem, Value>::dump_node(const typename LinkedTrie<KeyItem, Value>::Node& node) const
{
    std::cout << " index=" << node.index << std::flush;
    if (node.index)
        std::cout << " key_item=" << std::hex << static_cast<int>(node.key_item) << std::flush;
    else
        std::cout << " key_item=<none>" << std::flush;
    std::cout << " child_index=" << node.child_index << std::flush;
    std::cout << " sibling_index=" << node.sibling_index << std::flush;
    std::cout << " value_index=" << node.value_index << std::flush;
    if (node.value_index)
        std::cout << " value=<something>" << std::endl;
    else
        std::cout << " value=<none>" << std::endl;
}

template<typename KeyItem, typename Value>
bool LinkedTrie<KeyItem, Value>::contains(const KeyItem* key, size_t key_len) const
{
    assert(key != nullptr);
    const KeyItem* key_item = key;

    const Node* node = root();
    for (size_t i = 0; i < key_len; ++i) {
        auto* child = child_of(*node);
        if (child == nullptr)
            return false;
        while (*key_item != child->key_item) {
            child = sibling_of(*child);
            if (child == nullptr)
                return false;
        }
        node = child;
        ++key_item;
    }

    return true;
}

template<typename KeyItem, typename Value>
const Value* LinkedTrie<KeyItem, Value>::get(const KeyItem* key, size_t key_len) const
{
    assert(key != nullptr);
    const KeyItem* key_item = key;

    const Node* node = root();
    for (size_t i = 0; i < key_len; ++i) {
        auto* child = child_of(*node);
        if (child == nullptr)
            return nullptr;
        while (*key_item != child->key_item) {
            child = sibling_of(*child);
            if (child == nullptr)
                return nullptr;
        }
        node = child;
        ++key_item;
    }

    return value_for(*node);
}

template<typename KeyItem, typename Value>
void LinkedTrie<KeyItem, Value>::set(const KeyItem* key, size_t key_len, Value value)
{
    // NB: We ned to use complicated indexing instead of raw pointers here since the
    //     vector might change location in memory under our feet (this happens if we
    //     force it to resize).
    const KeyItem* key_item = key;
    size_t node_index = 0;
    size_t child_index;
    size_t sibling_index;

    const auto node = [&]() -> Node* { return node_at(node_index); };
    const auto child = [&]() -> Node* { return node_at(child_index); };
    const auto sibling = [&]() -> Node* { return node_at(sibling_index); };

    // Iterate thru until node_index points to either an existing or newly created Node
    for (size_t i = 0; i < key_len; ++i) {
        assert(key_item != nullptr);
        child_index = node()->child_index;
        if (child_index == 0) {
            // End of this line of children
            child_index = make_node(*key_item).index;
            node()->child_index = child_index;
        }
        while (*key_item != child()->key_item) {
            sibling_index = child()->sibling_index;
            if (sibling_index == 0) {
                // Append to this line of siblings
                sibling_index = make_node(*key_item).index;
                child()->sibling_index = sibling_index;
            } else if (*key_item < sibling()->key_item) {
                // Insert between existing siblings
                auto next_sibling_index = sibling_index;
                sibling_index = make_node(*key_item).index;
                child()->sibling_index = sibling_index;
                sibling()->sibling_index = next_sibling_index;
            } else {
                // Keep searching along this line of siblings
            }
            child_index = sibling_index;
        }
        node_index = child_index;
        ++key_item;
    }

    // Actually save the value, and set the Node's index to point to it
    m_values.push_back(std::move(value));
    node()->value_index = m_values.size() - 1;

#ifdef LINKED_TRIE_DEBUG
    std::cout << "Dumping..." << std::endl;
    for (auto i = 0; i < m_nodes.size(); ++i) {
        std::cout << "node(" << i << "):" << std::flush;
        dump_node(*node_at(i));
    }
#endif
}

template<typename KeyItem, typename Value>
typename LinkedTrie<KeyItem, Value>::Node& LinkedTrie<KeyItem, Value>::make_node(KeyItem key_item)
{
    auto new_index = m_nodes.size();
    m_nodes.push_back({ key_item, new_index, 0, 0, 0 });
    return m_nodes.back();
}
