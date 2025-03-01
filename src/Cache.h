#ifndef CACHE_H
#define CACHE_H

#include "ICache.h"
#include "types/ReplPolicies.h"

#include <vector>

class Cache : public ICache
{
private:
	void ProcessCacheHit(Instruction instruction);
	void ProcessCacheMiss(Instruction instruction);

public:
	int size, associativity, blockSize;
	int numSets;

	ReplacementPolicy replacement;

	std::vector<std::vector<Address>> tags;

	Cache(int size, int associativity, int blockSize, ReplacementPolicy replacement);

	bool ProcessRequest(Instruction instruction) override;
	void Evict(Address address) override;
	void Issue(Address address) override;
};

#endif
