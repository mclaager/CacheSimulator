#ifndef MEMORYHIERARCHY_H
#define MEMORYHIERARCHY_H

#include <vector>
#include <memory>

#include "types/MemoryHierarchyStatistics.h"
#include "ICache.h"

class MemoryHierarchy
{
public:
    MemoryHierarchyStatistics statistics;
    std::vector<std::shared_ptr<ICache>> cacheModules;

    MemoryHierarchy(std::vector<std::shared_ptr<ICache>> cacheModules);

    bool ProcessRequest(Instruction instruction);
};

#endif