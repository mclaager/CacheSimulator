#ifndef INSTRUCTION_H
#define INSTRUCTION_H

// int == 32 bit
typedef unsigned int Address;

enum MemoryOperation
{
    Read = 0,
    Write = 1
};

struct Instruction
{
    Address address;
    MemoryOperation operation;

    // The amount of cycles until this operation is used again, if known
    int cyclesUntilReuse;
};

#endif