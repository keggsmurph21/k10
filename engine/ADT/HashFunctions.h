#pragma once

namespace ADT {

inline unsigned int_hash(int key)
{
    // how does this magic work .....
    key += ~(key << 15);
    key ^= (key >> 10);
    key += (key << 3);
    key ^= (key >> 6);
    key += ~(key << 11);
    key ^= (key >> 16);
    return key;
}

} // namespace ADT
