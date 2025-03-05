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