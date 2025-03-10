#include "Cache.h"

#include <iomanip>
#include <sstream>

#include <iostream>

Cache::Cache(int size, int associativity, int blockSize, ReplacementPolicy replacement, std::string name)
	: size(size), associativity(associativity), blockSize(blockSize),
	replacement(replacement)
{
	Cache::name = name;

	Cache::numSets = size / (associativity * blockSize);

	Cache::memory.resize(numSets, std::vector<CacheLine>(associativity));

	switch (Cache::replacement)
	{
		case ReplacementPolicy::LRU:
			Cache::replacementData.resize(numSets, std::vector<unsigned int>(associativity, 0));
			break;
		case ReplacementPolicy::FIFO:
			Cache::replacementData.resize(numSets, std::vector<unsigned int>(1, 0));
			break;
		case ReplacementPolicy::OPTIMAL:
			Cache::replacementData.resize(numSets, std::vector<unsigned int>(associativity, NEVER_REUSED));
			break;
	}
	
}

CacheRequestOutput Cache::ProcessCacheHit(Instruction instruction, unsigned int set, unsigned int associativityIdx)
{
	// Set the dirty bit flag for a cache hit (Write-back policy)
	if (instruction.operation == MemoryOperation::Write)
		memory[set][associativityIdx].isDirty = true;

	switch (Cache::replacement)
	{
		case ReplacementPolicy::FIFO:
			break;
		case ReplacementPolicy::LRU:
			// TODO: Implement LRU Replacement Policy for Cache Hit
			break;
		case ReplacementPolicy::OPTIMAL:
			Cache::replacementData[set][associativityIdx] = instruction.cyclesUntilReuse;
			break;
	}

	// cache data at set[index] would be returned for a real cache
	CacheRequestOutput output =
	{
		.status = CacheHit,
		.address = instruction.address
	};
	return output;
}

CacheRequestOutput Cache::ProcessCacheMiss(Instruction instruction, unsigned int set)
{
	int i;

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

	unsigned int replacementIdx = 0;

	// Check if cache has unoccupied slots (if so, use the first one)
	for (i = 0; i < Cache::associativity; i++)
	{
		if (!Cache::memory[set][i].isOccupied)
		{
			replacementIdx = i;
			break;
		}
	}

	// If not, perform a replacement policy
	if (!Cache::memory[set][i].isOccupied)
	{
		switch (Cache::replacement)
		{
			case ReplacementPolicy::FIFO:
				replacementIdx = replacementData[set][0];
				replacementData[set][0] = (replacementData[set][0] + 1) % Cache::associativity;
				break;
			case ReplacementPolicy::LRU:
				// TODO: Implement LRU Replacement Policy for Cache Miss
				break;
			case ReplacementPolicy::OPTIMAL:
				// Gets the index of the farthest away used address
				unsigned int farthestBlockValue = 0;
				for (i = 0; i < Cache::associativity; i++)
				{
					if (Cache::replacementData[set][i] > farthestBlockValue)
					{
						replacementIdx = i;
						farthestBlockValue = Cache::replacementData[set][i];
						// Early break (always replace first block that's never reused)
						if (farthestBlockValue == NEVER_REUSED)
							break;
					}
				}
				Cache::replacementData[set][replacementIdx] = instruction.cyclesUntilReuse;
				break;
		}	
	}

	PerformWriteBack(set, replacementIdx, instruction.operation);

	// Replaces the cache tag with new tag
	bool isReplacingAddress = Cache::memory[set][replacementIdx].isOccupied;
	Address originalAddress = Cache::memory[set][replacementIdx].originalAddress;

	Cache::memory[set][replacementIdx].originalAddress = instruction.address;
	Cache::memory[set][replacementIdx].tag = Cache::ToTag(instruction.address);
	Cache::memory[set][replacementIdx].isOccupied = true;

	CacheRequestOutput output =
	{
		.status = isReplacingAddress ? CacheMissNoEviction : CacheMissEviction,
		.address = originalAddress
	};
	return output;
}

CacheRequestOutput Cache::ProcessRequest(Instruction instruction)
{
	// Determine the set the instruction belongs to
	unsigned int set = GetSet(instruction.address);

	bool isHit = false;

	int i;
	for (i = 0; i < Cache::associativity; i++)
	{
		// Check if the current address block is hit
		if (Cache::ToTag(instruction.address) == Cache::memory[set][i].tag)
		{
			isHit = true;
			break;
		}
	}

	CacheRequestOutput output = isHit ?
		Cache::ProcessCacheHit(instruction, set, i) :
		Cache::ProcessCacheMiss(instruction, set);

	// Post-process for replacement policies
	switch (Cache::replacement)
	{
		case ReplacementPolicy::OPTIMAL:
			unsigned int set;
			for (set = 0; set < Cache::numSets; set++)
			{
				// std::cout << "Data: ";
				// for (i = 0; i < Cache::associativity; i++)
				// {
				// 	std::cout << Cache::replacementData[set][i] << " ";
				// }
				// std::cout << std::endl;
				unsigned int associativityIdx;
				for (associativityIdx = 0; associativityIdx < Cache::associativity; associativityIdx++)
				{
					if (Cache::replacementData[set][associativityIdx] != NEVER_REUSED)
						Cache::replacementData[set][associativityIdx]--;
				}
			}
			break;
	}

	// std::cout << Cache::ToString() << std::endl;

	return output;
}

unsigned int Cache::GetSet(Address address)
{
	return (address / Cache::blockSize) % Cache::numSets;
}

Tag Cache::ToTag(Address address)
{
	return address / (Cache::blockSize * Cache::numSets);
}

void Cache::PerformWriteBack(unsigned int set, unsigned int associativityIdx, MemoryOperation operation)
{
	// Perform the "write"
	if (Cache::memory[set][associativityIdx].isDirty)
		Cache::statistics.writes++;

	// Update the dirty bit to the appropriate state
	Cache::memory[set][associativityIdx].isDirty = operation == MemoryOperation::Write;
}

std::string Cache::ToString()
{
	std::string str = "";
	std::stringstream ss;

	ss << "===== " << name << " contents =====\n";
	str.append(ss.str());
	ss.str("");
	ss.clear();

	unsigned int set;
	unsigned int associativityIdx;
	for (set = 0; set < Cache::numSets; set++)
	{
		std::string numStr = std::to_string(set) + ":";
		ss << "Set     " << std::setw(8) << std::left << numStr;
		str.append(ss.str());
		ss.str("");
		ss.clear();

		for (associativityIdx = 0; associativityIdx < Cache::associativity; associativityIdx++)
		{
			ss << std::hex << memory[set][associativityIdx].tag << std::dec << " "
				<< (memory[set][associativityIdx].isDirty ? "D" : " ") << "   ";
			str.append(ss.str());
			ss.str("");
			ss.clear();
		}
		str.append("\n");
	}

	return str;
}

void Cache::Evict(Address address)
{
	// Determine the set the instruction belongs to
	unsigned int set = GetSet(address);

	int i;
	for (i = 0; i < Cache::associativity; i++)
	{
		// Check if the current address block is hit
		if (Cache::ToTag(address) == Cache::memory[set][i].tag)
		{
			// Reset line of cache memory
			Cache::memory[set][i] = CacheLine();
			return;
		}
	}

	// For evicting address in higher levels of cache for inclusive memory hierarchies,
	// this return should not be reachable. 
	return;
}
