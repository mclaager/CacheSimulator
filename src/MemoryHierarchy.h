#ifndef MEMORYHIERARCHY_H
#define MEMORYHIERARCHY_H

#include <vector>
#include <memory>
#include <string>

#include "types/MemoryHierarchyStatistics.h"
#include "ICache.h"

class MemoryHierarchy
{
public:
    MemoryHierarchyStatistics statistics;
    std::vector<std::shared_ptr<ICache>> cacheModules;

    MemoryHierarchy(std::vector<std::shared_ptr<ICache>> cacheModules);

    std::string ToString();

    // Processes the instruction on each level of the hierarchy. Returns true if cache hit, otherwise false.
    bool ProcessRequest(Instruction instruction);
};

#endif
