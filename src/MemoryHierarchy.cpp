#include "MemoryHierarchy.h"

#include <sstream>
#include <cmath>

MemoryHierarchy::MemoryHierarchy(std::vector<std::shared_ptr<ICache>> cacheModules, bool isInclusive) :
	isInclusive(isInclusive)
{
	for (int i = 0; i < cacheModules.size(); i++)
	{
		cacheModules[i]->isInclusive = isInclusive;
		MemoryHierarchy::cacheModules.push_back(cacheModules[i]);
		if (i > 0)
		{
			MemoryHierarchy::cacheModules[i-1]->next = MemoryHierarchy::cacheModules[i];
			MemoryHierarchy::cacheModules[i]->prev = MemoryHierarchy::cacheModules[i-1];
		}
	}
}

bool MemoryHierarchy::ProcessRequest(Instruction instruction)
{
	// Make a copy to allow for potential updates
	Instruction instructionCopy = instruction;

	int i;
	for (i = 0; i < cacheModules.size(); i++)
	{
		CacheRequestOutput output = cacheModules[i]->ProcessRequest(instructionCopy);

		// If cache hit, no need to process other caches
		if(output.status == CacheHit)
		{
			return true;
		}
		// If an eviction occured for inclusive caches, remove all other instances of the block from upper levels of hierarchy
		else if (i > 0 && MemoryHierarchy::isInclusive && output.status == CacheMissEviction)
		{
			for (int j = i - 1; j > -1; j--)
			{
				cacheModules[j]->Evict(output.address);
			}
		}

		// Writes to lower cache levels should be reads in normal circumstances, even if L1 was a write (because of write-back)
		instructionCopy.operation = MemoryOperation::Read;
	}
	return false;
}


std::string MemoryHierarchy::ToString()
{
	std::string str = "";

	int i;
	for (i = 0; i < cacheModules.size(); i++)
	{
		str.append(cacheModules[i]->ToString());
	}

	return str;
}

std::string MemoryHierarchy::StatisticsOutput()
{
	std::string str = "===== Simulation results (raw) =====\n";

	std::stringstream ss;

	char currentLineIdentifier = 'a';
	for (auto cache : MemoryHierarchy::cacheModules)
	{
		str += cache->StatisticsOutput(currentLineIdentifier);
		currentLineIdentifier += 6;
	}

	// Pad with default L2 output if L2 doesn't exist
	if (MemoryHierarchy::cacheModules.size() == 1)
	{
		str += "g. number of L2 reads:        0\n";
		str += "h. number of L2 read misses:  0\n";
		str += "i. number of L2 writes:       0\n";
		str += "j. number of L2 write misses: 0\n";
		str += "k. L2 miss rate:              0\n";
		str += "l. number of L2 writebacks:   0\n";

		currentLineIdentifier += 6;
	}

	unsigned int totalMemoryTraffic = 0;
	if (MemoryHierarchy::cacheModules.size() > 0)
	{
		auto lastStats = MemoryHierarchy::cacheModules.back()->statistics;
		totalMemoryTraffic = lastStats.readMisses + lastStats.writeMisses + lastStats.writeBacks + lastStats.writePropagations;
	}

	// Print total memory traffic for simulation
	ss << currentLineIdentifier++ << ". total memory traffic:      " << totalMemoryTraffic << std::endl;

	str.append(ss.str());
	ss.str("");
	ss.clear();

	return str;
}