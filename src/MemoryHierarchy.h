#ifndef MEMORYHIERARCHY_H
#define MEMORYHIERARCHY_H

#include <vector>

#include "types/MemoryHierarchyStatistics.h"
#include "ICache.h"

class MemoryHierarchy
{
public:
    MemoryHierarchyStatistics statistics;
    std::vector<ICache> cacheModules;

    MemoryHierarchy(std::vector<ICache> cacheModules);

    bool ProcessRequest(Instruction instruction);
};

#endif