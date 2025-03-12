#include "GraphCache.h"

GraphCache::GraphCache(std::string name, unsigned int blockSize, unsigned int numSets, unsigned int maxSize)
    : maxSize(maxSize), numSets(numSets), blockSize(blockSize)
{
    GraphCache::name = name;

    GraphCache::currentSize = 0;
}

CacheRequestOutput GraphCache::ProcessRequest(Instruction instruction)
{
    return CacheRequestOutput();
}

CacheRequestOutput GraphCache::ProcessCacheHit(Instruction instruction, unsigned int set, unsigned int associativityIdx)
{
    return CacheRequestOutput();
}

CacheRequestOutput GraphCache::ProcessCacheMiss(Instruction instruction, unsigned int set)
{
    return CacheRequestOutput();
}

unsigned int GraphCache::GetSet(Address address)
{
	return (address / GraphCache::blockSize) % GraphCache::numSets;
}

Tag GraphCache::ToTag(Address address)
{
	return address / (GraphCache::blockSize * GraphCache::numSets);
}

void GraphCache::Evict(Address address)
{
}

std::string GraphCache::ToString()
{
    return std::string();
}
