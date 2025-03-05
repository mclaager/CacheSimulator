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

	Cache::tags.resize(numSets, std::vector<unsigned int>(associativity, 0));
	Cache::dirtyBits.resize(numSets, std::vector<bool>(associativity, false));
	Cache::occupiedBits.resize(numSets, std::vector<bool>(associativity, false));

	switch (Cache::replacement)
	{
		case ReplacementPolicy::OPTIMAL:
			Cache::replacementData.resize(numSets, std::vector<unsigned int>(associativity, NEVER_REUSED));
			break;
	}
	
}

void Cache::ProcessCacheHit(Instruction instruction, unsigned int set, unsigned int associativityIdx)
{
	// Set the dirty bit flag for a cache hit (Write-back policy)
	if (instruction.operation == MemoryOperation::Write)
		dirtyBits[set][associativityIdx] = true;

	switch (Cache::replacement)
	{
		case ReplacementPolicy::FIFO:
			// TODO: Implement FIFO Replacement Policy for Cache Hit
			break;
		case ReplacementPolicy::LRU:
			// TODO: Implement LRU Replacement Policy for Cache Hit
			break;
		case ReplacementPolicy::OPTIMAL:
			Cache::replacementData[set][associativityIdx] = instruction.cyclesUntilReuse;
			break;
	}

	// cache data at set[index] would be returned for a real cache
}

void Cache::ProcessCacheMiss(Instruction instruction, unsigned int set)
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
		if (!Cache::occupiedBits[set][i])
		{
			replacementIdx = i;
			break;
		}
	}

	// If not, perform a replacement policy
	if (replacementIdx == 0)
	{
		switch (Cache::replacement)
		{
			case ReplacementPolicy::FIFO:
				// TODO: Implement FIFO Replacement Policy for Cache Miss
				break;
			case ReplacementPolicy::LRU:
				// TODO: Implement LRU Replacement Policy for Cache Miss
				break;
			case ReplacementPolicy::OPTIMAL:
				// Gets the index of the farthest away used address
				int farthestBlockValue = -1;
				for (i = 0; i < Cache::associativity; i++)
				{
					if (Cache::replacementData[set][i] >= farthestBlockValue)
					{
						replacementIdx = i;
						farthestBlockValue = Cache::replacementData[set][i];
						if (farthestBlockValue == NEVER_REUSED)
							break;
					}
				}
				break;
		}	
	}

	PerformWriteBack(set, replacementIdx, instruction.operation);
			
	Cache::replacementData[set][replacementIdx] = instruction.cyclesUntilReuse;

	// Replaces the cache tag with new tag
	Cache::tags[set][replacementIdx] = instruction.address / Cache::blockSize;
	Cache::occupiedBits[set][replacementIdx] = true;
}

bool Cache::ProcessRequest(Instruction instruction)
{
	// Determine the set the instruction belongs to
	unsigned int set = GetSet(instruction);

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
		Cache::ProcessCacheHit(instruction, set, i);
	else
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

	return isHit;
}

unsigned int Cache::GetSet(Instruction instruction)
{
	return (instruction.address / Cache::blockSize) % Cache::numSets;
}

void Cache::PerformWriteBack(unsigned int set, unsigned int associativityIdx, MemoryOperation operation)
{
	// Perform the "write"
	if (Cache::dirtyBits[set][associativityIdx])
		Cache::statistics.writes++;

	// Update the dirty bit to the appropriate state
	Cache::dirtyBits[set][associativityIdx] = operation == MemoryOperation::Write;
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
			ss << std::hex << tags[set][associativityIdx] << std::dec << " "
				<< (dirtyBits[set][associativityIdx] ? "D" : " ") << "   ";
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
	
}

void Cache::Issue(Address address)
{
	
}
