#ifndef MEMORYHIERARCHY_H
#define MEMORYHIERARCHY_H

#include <vector>
#include <memory>
#include <string>

#include "ICache.h"
#include "GraphCache.h"

class MemoryHierarchy
{
private:
    Block ToBlock(Address address);
public:
    std::vector<std::shared_ptr<ICache>> cacheModules;
    bool isInclusive;

    // Prefetching related variables
    Graph prefetchGraph;
	bool didFetch;
	Block lastBlock, previousFetch;
    unsigned int blockSize; // Prefetching works on blocks

    // Statistics for prefetching
    unsigned int totalPredictions;
    unsigned int uniqueCorrectPredictionsL1;
    unsigned int sharedCorrectPredictionsL1;
    unsigned int uniqueCorrectPredictionsL2;
    unsigned int sharedCorrectPredictionsL2;

    MemoryHierarchy(std::vector<std::shared_ptr<ICache>> cacheModules, bool isInclusive, GraphLimitingQueue* queue, unsigned int blockSize);

    std::string ToString();

    std::string StatisticsOutput();

    // Processes the instruction on each level of the hierarchy. Returns true if cache hit, otherwise false.
    bool ProcessRequest(Instruction instruction);

    // Perform all prefetching checks and operations
    void PerformPrefetching(Instruction instruction, CacheRequestOutput output);
};

#endif
