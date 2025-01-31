#ifndef MEMORYHIERARCHYSTATISTICS_H
#define MEMORYHIERARCHYSTATISTICS_H

#include <vector>

struct MemoryHierarchyStatistics
{
    // Write Backs per each Cache in Hierarchy
    std::vector<int> writeBacks;
};

#endif