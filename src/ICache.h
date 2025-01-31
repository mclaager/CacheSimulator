#ifndef ICACHE_H
#define ICACHE_H

#include "types/Instruction.h"
#include "types/CacheStatistics.h"

class ICache
{
public:
    CacheStatistics statistics;

    // Process a read/write request from previous stage (CPU, L1, etc.)
    virtual bool ProcessRequest(Instruction instruction) = 0;
    // Manually evict an address from this module
    virtual void Evict(Address address) = 0;
    // Manually issue a block in module
    virtual void Issue(Address address) = 0;
};

#endif