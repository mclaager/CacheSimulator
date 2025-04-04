#ifndef ICACHE_H
#define ICACHE_H

#include "types/Instruction.h"
#include "types/CacheStatistics.h"
#include "types/CacheRequestOutput.h"

#include <string>
#include <memory>
#include <cstdint>

class ICache
{
public:
    std::shared_ptr<ICache> prev;
    std::shared_ptr<ICache> next;
    CacheStatistics statistics;
    std::string name;
    unsigned int correctPredictions;

    bool isInclusive;

    // Process a read/write request from previous stage (CPU, L1, etc.)
    virtual CacheRequestOutput ProcessRequest(Instruction instruction) = 0;
    // Manually evict an address from this module
    virtual void Evict(Address address) = 0;
    // Process writes that need to be sent to the main memory
    virtual void PropagateWriteToMainMemory() = 0;

    virtual std::string ToString() = 0;
    virtual std::string StatisticsOutput(char startingLineIdentifier) = 0;
};

#endif