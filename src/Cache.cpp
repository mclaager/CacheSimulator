#include "Cache.h"

Cache::Cache(int size, int associativity, int blockSize) : size(size),
	associativity(associativity), blockSize(blockSize)
{
	
}

bool Cache::ProcessRequest(Instruction instruction)
{
	return true;
}

void Cache::Evict(Address address)
{
	
}

void Cache::Issue(Address address)
{
	
}
