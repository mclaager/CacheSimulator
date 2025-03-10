#ifndef CACHEADDRESSDATA_H
#define CACHEADDRESSDATA_H

#include "Instruction.h"

typedef unsigned int Tag;

// The data and metadata held at each "cell" within a cache module
struct CacheLine
{
    // The smaller representation of the address to determine placement in the 
    Tag tag = 0;

    // required for inclusive memory hierarchy
    Address originalAddress = 0;

    bool isOccupied = false;
    bool isDirty = false;
};

#endif