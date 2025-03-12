#ifndef GRAPHCACHE_H
#define GRAPHCACHE_H

#include "ICache.h"
#include "types/CacheLine.h"

#include <vector>
#include <map>
#include <sstream>

class GraphNode
{
public:
    bool isDirty;
};

class Graph
{
public:
    std::map<Tag, GraphNode> nodes;

    std::string ToString()
    {
        std::string output = "";
        std::stringstream ss;
        for (auto kvp : nodes)
        {
            ss << "[";
        }
    };
};

class GraphCache : public ICache
{
private:
	CacheRequestOutput ProcessCacheHit(Instruction instruction, unsigned int set, unsigned int associativityIdx);
	CacheRequestOutput ProcessCacheMiss(Instruction instruction, unsigned int set);

	// Get the cache set for an address
	unsigned int GetSet(Address address);
	// Perform the write-back operation for a block of data in the cache, if applicable
	// void PerformWriteBack(unsigned int set, unsigned int associativityIdx, MemoryOperation operation);

	Tag ToTag(Address address);

public:
	unsigned int maxSize, numSets, blockSize;
    unsigned int currentSize;
	int numSets;

	// A container that holds the cache line data for each entry
	std::vector<Graph> memory;

	GraphCache(std::string name, unsigned int associativity, unsigned int blockSize, unsigned int maxSize = __UINT32_MAX__);

	CacheRequestOutput ProcessRequest(Instruction instruction) override;
	void Evict(Address address) override;

	std::string ToString() override;
};

#endif
