#include "MemoryHierarchy.h"

#include <sstream>
#include <cmath>

MemoryHierarchy::MemoryHierarchy(std::vector<std::shared_ptr<ICache>> cacheModules, bool isInclusive, GraphLimitingQueue* queue, unsigned int blockSize) :
	isInclusive(isInclusive), prefetchGraph(queue), blockSize(blockSize)
{
	MemoryHierarchy::totalPredictions = 0;
	MemoryHierarchy::uniqueCorrectPredictionsL1 = 0;
	MemoryHierarchy::sharedCorrectPredictionsL1 = 0;
	MemoryHierarchy::uniqueCorrectPredictionsL2 = 0;
	MemoryHierarchy::sharedCorrectPredictionsL2 = 0;

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

	bool l1Miss = false;

	PerformPrefetching(instructionCopy);

	int i;
	CacheRequestOutput output;
	for (i = 0; i < cacheModules.size(); i++)
	{
		output = cacheModules[i]->ProcessRequest(instructionCopy);

		if (previousFetch == MemoryHierarchy::ToBlock(instruction.address))
		{
			if (output.status == CacheHit)
			{
				if (output.sender == "L1")
					MemoryHierarchy::sharedCorrectPredictionsL1++;
				else if (output.sender == "L2")
					MemoryHierarchy::sharedCorrectPredictionsL2++;
			}
			else
			{
				if (output.sender == "L1")
					MemoryHierarchy::uniqueCorrectPredictionsL1++;
				else if (output.sender == "L2")
					MemoryHierarchy::uniqueCorrectPredictionsL2++;
			}
		}

		// // Perform prefetching only on L1 misses
		// if(i == 0 && output.status != CacheHit)
		// 	l1Miss = true;
		// if (i == (cacheModules.size() - 1) && l1Miss)
		// 	PerformPrefetching(instructionCopy, output);

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

void MemoryHierarchy::PerformPrefetching(Instruction instruction)
{
	// If queue was never set, don't perform prefetching.
	if (!prefetchGraph.IsValid())
		return;
	
	MemoryHierarchy::totalPredictions++;

	// Try to add the currently accessed address
	prefetchGraph.AddNode(MemoryHierarchy::ToBlock(instruction.address));

	if (MemoryHierarchy::didFetch)
	{
		
		//std::cout << "Correct? " << (Cache::previousFetch == instruction.address) << std::endl;
		if (previousFetch == MemoryHierarchy::ToBlock(instruction.address))
		{
			//std::cout<<"Predicted Correctly: "<< std::dec<< ICache::correctPredictions <<std::endl;
			//std::cout<<"predict: "<<std::hex<<previousFetch<<" actual: "<<std::hex<<instruction.address<<std::endl;
			prefetchGraph.HandleCorrectPrediction(MemoryHierarchy::lastBlock, MemoryHierarchy::previousFetch);
		}
		else
		{
			//std::cout<<"given: "<<std::hex<<Cache::lastAddress<<" incorrect predict: "<<std::hex<<previousFetch<<" actual: "<<std::hex<<instruction.address<<std::endl;
			//std::cout<<"calling Handle incorrect prediction"<<std::endl;
			prefetchGraph.HandleIncorrectPrediction(MemoryHierarchy::lastBlock, MemoryHierarchy::previousFetch);
		}
	}

	Block fetchedBlock = prefetchGraph.PrefetchBlock(MemoryHierarchy::ToBlock(instruction.address));
	MemoryHierarchy::didFetch = fetchedBlock != __UINT32_MAX__;
	if (MemoryHierarchy::didFetch)
	{
		MemoryHierarchy::previousFetch = fetchedBlock;
		MemoryHierarchy::lastBlock = MemoryHierarchy::ToBlock(instruction.address);
	}
}

Block MemoryHierarchy::ToBlock(Address address)
{
	return (address & ~(MemoryHierarchy::blockSize -1));
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

	// If no prefetching, return these statistics
	if (!prefetchGraph.IsValid())
		return str;

	str += "===== Prefetching results (raw) =====\n";

	ss << "a. number of prefetcher update calls:    " << MemoryHierarchy::totalPredictions << std::endl;
	ss << "b. unique correct predictions on L1:     " << MemoryHierarchy::uniqueCorrectPredictionsL1 << std::endl;
	ss << "c. non-unique correct predictions on L1: " << MemoryHierarchy::sharedCorrectPredictionsL1 << std::endl;
	ss << "d. unique correct predictions on L2:     " << MemoryHierarchy::uniqueCorrectPredictionsL2 << std::endl;
	ss << "e. non-unique correct predictions on L2: " << MemoryHierarchy::sharedCorrectPredictionsL2 << std::endl;

	str.append(ss.str());
	ss.str("");
	ss.clear();

	return str;
}