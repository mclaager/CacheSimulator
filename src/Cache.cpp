#include "Cache.h"

Cache::Cache(int size, int associativity, int blockSize, ReplacementPolicy replacement)
	: size(size), associativity(associativity), blockSize(blockSize),
	replacement(replacement)
{
	Cache::numSets = size / (associativity * blockSize);

	Cache::tags.resize(numSets, std::vector<Address>(associativity, 0));
}

bool Cache::ProcessRequest(Instruction instruction)
{
	Address addr = instruction.address;

	return true;
}

void Cache::Evict(Address address)
{
	
}

void Cache::Issue(Address address)
{
	
}
