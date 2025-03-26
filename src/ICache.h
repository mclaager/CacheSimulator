#ifndef ICACHE_H
#define ICACHE_H

#include "types/Instruction.h"
#include "types/CacheStatistics.h"
#include "types/CacheRequestOutput.h"

#include <string>
#include <cstdint>

class ICache
{
public:
    CacheStatistics statistics;
    std::string name;
    unsigned int correctPredictions;

    // Process a read/write request from previous stage (CPU, L1, etc.)
    virtual CacheRequestOutput ProcessRequest(Instruction instruction) = 0;
    // Manually evict an address from this module
    virtual void Evict(Address address) = 0;

    virtual std::string ToString() = 0;
};

#endif