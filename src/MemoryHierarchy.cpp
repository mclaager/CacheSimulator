#include "MemoryHierarchy.h"

MemoryHierarchy::MemoryHierarchy(std::vector<std::shared_ptr<ICache>> cacheModules)
{
	for (int i = 0; i < cacheModules.size(); i++)
	{
		MemoryHierarchy::cacheModules.push_back(cacheModules[i]);
	}
}

/// <summary>
/// Processes the instruction on each level of the hierarchy.
/// </summary>
/// <returns>True if cache hit, otherwise False.</returns>
bool MemoryHierarchy::ProcessRequest(Instruction instruction)
{
	int i;
	for (i = 0; i < cacheModules.size(); i++)
	{
		if(cacheModules[i].get()->ProcessRequest(instruction))
		{
			return true;
		}
	}
	return false;
}
