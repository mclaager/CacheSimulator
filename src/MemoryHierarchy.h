#ifndef MEMORYHIERARCHY_H
#define MEMORYHIERARCHY_H

#include <vector>
#include <memory>
#include <string>

#include "ICache.h"

class MemoryHierarchy
{
public:
    std::vector<std::shared_ptr<ICache>> cacheModules;
    bool isInclusive;

    MemoryHierarchy(std::vector<std::shared_ptr<ICache>> cacheModules, bool isInclusive);

    std::string ToString();

    std::string StatisticsOutput();

    // Processes the instruction on each level of the hierarchy. Returns true if cache hit, otherwise false.
    bool ProcessRequest(Instruction instruction);
};

#endif
