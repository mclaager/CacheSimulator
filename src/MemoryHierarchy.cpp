#include "MemoryHierarchy.h"

MemoryHierarchy::MemoryHierarchy(std::vector<std::shared_ptr<ICache>> cacheModules, bool isInclusive) :
	isInclusive(isInclusive)
{
	for (int i = 0; i < cacheModules.size(); i++)
	{
		MemoryHierarchy::cacheModules.push_back(cacheModules[i]);
	}
}

bool MemoryHierarchy::ProcessRequest(Instruction instruction)
{
	int i;
	for (i = 0; i < cacheModules.size(); i++)
	{
		CacheRequestOutput output = cacheModules[i].get()->ProcessRequest(instruction);
		// If cache hit, no need to process other caches
		if(output.status == CacheHit)
		{
			return true;
		}
		// Otherwise, for inclusive caches, evict all evicted address from previous cache modules
		else if (i > 0 && MemoryHierarchy::isInclusive && output.status == CacheMissEviction)
		{
			for (int j = i - 1; j > -1; j--)
			{
				cacheModules[j].get()->Evict(output.address);
			}
		}
	}
	return false;
}


std::string MemoryHierarchy::ToString()
{
	std::string str = "";

	int i;
	for (i = 0; i < cacheModules.size(); i++)
	{
		str.append(cacheModules[i].get()->ToString());
	}

	return str;
}