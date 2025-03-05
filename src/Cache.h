#ifndef CACHE_H
#define CACHE_H

#include "ICache.h"
#include "types/ReplPolicies.h"

#include <vector>

class Cache : public ICache
{
private:
	void ProcessCacheHit(Instruction instruction, unsigned int set, unsigned int associativityIdx);
	void ProcessCacheMiss(Instruction instruction, unsigned int set);

	// Get the cache set for an instruction
	unsigned int GetSet(Instruction instruction);
	// Perform the write-back operation for a block of data in the cache, if applicable
	void PerformWriteBack(unsigned int set, unsigned int associativityIdx, MemoryOperation operation);

	unsigned int ToTag(Address address);

public:
	int size, associativity, blockSize;
	int numSets;

	ReplacementPolicy replacement;

	// A container that holds the tag data for each address
	std::vector<std::vector<unsigned int>> tags;
	// A container to store dirty write flags for each address
	std::vector<std::vector<bool>> dirtyBits;
	// A container for replacement policies to store arbitrary data for each address
	std::vector<std::vector<unsigned int>> replacementData;
	// A container to store whether each spot in the cache is occupied
	std::vector<std::vector<bool>> occupiedBits;

	Cache(int size, int associativity, int blockSize, ReplacementPolicy replacement, std::string name);

	bool ProcessRequest(Instruction instruction) override;
	void Evict(Address address) override;
	void Issue(Address address) override;

	std::string ToString() override;
};

#endif
