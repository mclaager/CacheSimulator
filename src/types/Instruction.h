#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <functional>

#define NEVER_REUSED __UINT32_MAX__
#define REUSE_NOT_APPLICABLE NEVER_REUSED

// Update how large addresses used in the cache are
typedef unsigned int Address;

// Define the size for certain attributes of addresses
typedef unsigned int Tag;
typedef unsigned int SetIndex;
typedef unsigned int AssociativityIndex;

typedef unsigned int Block;


enum MemoryOperation
{
    None = -1,

    Read = 0,
    Write = 1
};

struct Instruction
{
    Address address;
    MemoryOperation operation;
    
    // TODO: The following value don't really belong, but would require extensive refactoring to remove
    // A function for internally getting the reuse cycles for blocks, usually for the case of write-back.
    std::function<unsigned int(Address)> get_next_used_idx;
};

#endif