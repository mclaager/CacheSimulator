#include "Cache.h"

Cache::Cache(int size, int associativity, int blockSize, ReplacementPolicy replacement)
	: size(size), associativity(associativity), blockSize(blockSize),
	replacement(replacement)
{
	Cache::numSets = size / (associativity * blockSize);

	Cache::tags.resize(numSets, std::vector<Address>(associativity, 0));
}

void Cache::ProcessCacheHit(Instruction instruction)
{
	// TODO: It seems like there is some additional processing for cache hitting on a write
	// Look into if this should be applied here.

	switch (Cache::replacement)
	{
		case ReplacementPolicy::FIFO:
			// TODO: Implement FIFO Replacement Policy for Cache Hit
			break;
		case ReplacementPolicy::LRU:
			// TODO: Implement LRU Replacement Policy for Cache Hit
			break;
		case ReplacementPolicy::OPTIMAL:
			// TODO: Implement OPTIMAL Replacement Policy for Cache Hit
			break;
	}
}

void Cache::ProcessCacheMiss(Instruction instruction)
{
	// Process statistics for cache miss
	switch (instruction.operation)
	{
		case MemoryOperation::Read:
			Cache::statistics.readMisses++;
			break;
		case MemoryOperation::Write:
			Cache::statistics.writeMisses++;
			break;
	}

	switch (Cache::replacement)
	{
		case ReplacementPolicy::FIFO:
			// TODO: Implement FIFO Replacement Policy for Cache Miss
			break;
		case ReplacementPolicy::LRU:
			// TODO: Implement LRU Replacement Policy for Cache Miss
			break;
		case ReplacementPolicy::OPTIMAL:
			// TODO: Implement OPTIMAL Replacement Policy for Cache Miss
			break;
	}
}

bool Cache::ProcessRequest(Instruction instruction)
{
	// Determine the set the instruction belongs to
	int set = (instruction.address / Cache::blockSize) % Cache::numSets;

	bool isHit = false;

	int i;
	for (i = 0; i < Cache::associativity; i++)
	{
		// Check if the current address block is hit
		if (instruction.address / Cache::blockSize == Cache::tags[set][i])
		{
			isHit = true;
			break;
		}
	}

	if (isHit)
		Cache::ProcessCacheHit(instruction);
	else
		Cache::ProcessCacheMiss(instruction);

	return isHit;
}

void Cache::Evict(Address address)
{
	
}

void Cache::Issue(Address address)
{
	
}
