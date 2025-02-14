#include "MemoryHierarchy.h"

MemoryHierarchy::MemoryHierarchy(std::vector<std::shared_ptr<ICache>> cacheModules)
{
	for (int i = 0; i < cacheModules.size(); i++)
	{
		MemoryHierarchy::cacheModules.push_back(cacheModules[i]);
	}
}

bool MemoryHierarchy::ProcessRequest(Instruction instruction)
{
	return true;
}
