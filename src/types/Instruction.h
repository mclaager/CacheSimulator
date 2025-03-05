#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#define NEVER_REUSED __UINT32_MAX__
#define REUSE_NOT_APPLICABLE NEVER_REUSED

// int == 32 bit
typedef unsigned int Address;


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

    // The amount of cycles until this operation is used again, if known
    unsigned int cyclesUntilReuse;
};

#endif