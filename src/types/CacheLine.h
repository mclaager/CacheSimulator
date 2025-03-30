#ifndef CACHEADDRESSDATA_H
#define CACHEADDRESSDATA_H

#include "Instruction.h"

// The data and metadata held at each "cell" within a cache module
struct CacheLine
{
    // The smaller representation of the address to determine placement in the 
    Tag tag = 0;

    // A reference to what the original address that was stored here was. Most usedul for inclusive policy
    Address originalAddress = 0;

    bool isOccupied = false;
    bool isDirty = false;
};

#endif