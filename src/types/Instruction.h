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
    
    // TODO: The following values don't really belong here, but require extensive refactoring to put in correct spots.

    // Determines if the instruction was made within a cache module, usually for the case of write-back.
    bool internallyCreated;

    // The amount of cycles until this operation is used again, if known
    unsigned int cyclesUntilReuse;

    // A function for internally getting the reuse cycles for addresses, usually for the case of write-back.
    std::function<unsigned int(Address)> get_reuse_distance;
};

#endif