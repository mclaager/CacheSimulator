#ifndef CACHE_H
#define CACHE_H

#include "ICache.h"
#include "types/ReplPolicies.h"
#include "types/CacheLine.h"

#include <vector>

class Cache : public ICache
{
private:
	CacheRequestOutput ProcessCacheHit(Instruction instruction, SetIndex set, AssociativityIndex associativityIdx);
	CacheRequestOutput ProcessCacheMiss(Instruction instruction, SetIndex set);

	// Get the cache set for an address
	SetIndex GetSet(Address address);
	// Perform the write-back operation for a block of data in the cache, if applicable
	void PerformWriteBack(SetIndex set, AssociativityIndex associativityIdx, Instruction originalInstruction);

	Block ToBlock(Address address);
	Tag ToTag(Address address);

public:
	int size, associativity, blockSize;
	int numSets;

	ReplacementPolicy replacement;

	// A container that holds the cache line data for each entry
	std::vector<std::vector<CacheLine>> memory;
	// A container for replacement policies to store arbitrary data for each address
	std::vector<std::vector<unsigned int>> replacementData;

	Cache(int size, int associativity, int blockSize, ReplacementPolicy replacement, std::string name);

	CacheRequestOutput ProcessRequest(Instruction instruction) override;
	void Evict(Address address) override;

	std::string ToString() override;
};

#endif
