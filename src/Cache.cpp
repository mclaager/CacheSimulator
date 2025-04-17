#include "Cache.h"

#include <iomanip>
#include <sstream>
#include <iostream>
#include <cmath>

Cache::Cache(int size, int associativity, int blockSize, ReplacementPolicy replacement, std::string name)
	: size(size), associativity(associativity), blockSize(blockSize), replacement(replacement)
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

CacheRequestOutput Cache::ProcessCacheHit(Instruction instruction, SetIndex set, AssociativityIndex associativityIdx)
{
	// Set the dirty bit flag for a cache hit (Write-back policy)
	if (instruction.operation == MemoryOperation::Write)
		memory[set][associativityIdx].isDirty = true;

	// block data at set[index] would be returned for a real cache
	CacheRequestOutput output =
	{
		.status = CacheHit,
		.address = instruction.address,
		.sender = Cache::name
	};
	return output;
}

CacheRequestOutput Cache::ProcessCacheMiss(Instruction instruction, SetIndex set)
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

	AssociativityIndex replacementIdx = 0;

	// Check if cache has unoccupied slots (if so, use the first one)
	for (i = 0; i < Cache::associativity; i++)
	{
		if (!Cache::memory[set][i].valid)
		{
			replacementIdx = i;
			break;
		}
	}
	// range check
	if (i >= Cache::associativity)
	{
		i = Cache::associativity - 1;
	}

	unsigned int maxCyclesLastUsed, farthestBlockValue;
	// If not, perform a replacement policy
	if (Cache::memory[set][i].valid)
	{
		switch (Cache::replacement)
		{
			case ReplacementPolicy::FIFO:
				replacementIdx = replacementData[set][0];
				replacementData[set][0] = (replacementData[set][0] + 1) % Cache::associativity;
				break;
			case ReplacementPolicy::LRU:
				maxCyclesLastUsed = 0;
				for (i = 0; i < Cache::associativity; i++)
				{
					if (Cache::replacementData[set][i] > maxCyclesLastUsed)
					{
						replacementIdx = i;
						maxCyclesLastUsed = Cache::replacementData[set][i];
					}
				}
				break;
			case ReplacementPolicy::OPTIMAL:
				// Gets the index of the farthest away used address
				farthestBlockValue = 0;
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
				Cache::replacementData[set][replacementIdx] = instruction.get_next_used_idx(instruction.address);
				break;
		}	
	}

	// Write-back the soon-to-be evicted block
	if (Cache::memory[set][replacementIdx].isDirty)
		PerformWriteBack(set, replacementIdx, instruction);

	// Replaces the cache tag with new tag
	bool isReplacingAddress = Cache::memory[set][replacementIdx].valid;
	Address originalAddress = Cache::memory[set][replacementIdx].originalAddress;

	Cache::memory[set][replacementIdx].originalAddress = instruction.address;
	Cache::memory[set][replacementIdx].tag = Cache::ToTag(instruction.address);
	Cache::memory[set][replacementIdx].valid = true;

	// Update the dirty bit to the appropriate state
	Cache::memory[set][replacementIdx].isDirty = instruction.operation == MemoryOperation::Write;

	if (isReplacingAddress && Cache::isInclusive && prev)
		prev->Evict(originalAddress);

	CacheRequestOutput output =
	{
		.status = isReplacingAddress ? CacheMissEviction : CacheMissNoEviction,
		.address = originalAddress,
		.sender = Cache::name
	};
	return output;
}

CacheRequestOutput Cache::ProcessRequest(Instruction instruction)
{
	// Update statistics
	switch (instruction.operation)
	{
		case MemoryOperation::Read: Cache::statistics.reads++; break;
		case MemoryOperation::Write: Cache::statistics.writes++; break;
	}

	// Determine the set the instruction belongs to
	SetIndex set = GetSet(instruction.address);

	bool isHit = false;

	int i;
	for (i = 0; i < Cache::associativity; i++)
	{
		// Check if the current address block is hit
		if ((Cache::ToTag(instruction.address) == Cache::memory[set][i].tag) && Cache::memory[set][i].valid)
		{
			isHit = true;
			break;
		}
	}

	CacheRequestOutput output = isHit ?
		Cache::ProcessCacheHit(instruction, set, i) :
		Cache::ProcessCacheMiss(instruction, set);

	unsigned int associativityIdx;
	// Post-process for replacement policies
	switch (Cache::replacement)
	{
		case ReplacementPolicy::LRU:
			for (associativityIdx = 0; associativityIdx < Cache::associativity; associativityIdx++)
			{
				if (!Cache::memory[set][associativityIdx].valid)
					continue;
				
				if (Cache::memory[set][associativityIdx].tag == Cache::ToTag(instruction.address))
					Cache::replacementData[set][associativityIdx] = 0;
				else
					Cache::replacementData[set][associativityIdx]++;
			}
			break;
		case ReplacementPolicy::OPTIMAL:
			for (associativityIdx = 0; associativityIdx < Cache::associativity; associativityIdx++)
			{
				if (!Cache::memory[set][associativityIdx].valid)
					continue;
				
				if (Cache::memory[set][associativityIdx].tag == Cache::ToTag(instruction.address))
					Cache::replacementData[set][associativityIdx] = instruction.get_next_used_idx(instruction.address);
			}
			break;
	}

	// std::cout << Cache::ToString() << std::endl;

	return output;
}

SetIndex Cache::GetSet(Address address)
{
	return (address / Cache::blockSize) % Cache::numSets;
}

Block Cache::ToBlock(Address address)
{
	return address / Cache::blockSize;
}

Tag Cache::ToTag(Address address)
{
	return address / (Cache::blockSize * Cache::numSets);
}

void Cache::PerformWriteBack(SetIndex set, AssociativityIndex associativityIdx, Instruction originalInstruction)
{
	// Update statistics
	Cache::statistics.writeBacks++;

	// Send a write to the next lower level of the cache
	if (next)
	{
		Instruction writeBackInstruction = 
		{
			.address = Cache::memory[set][associativityIdx].originalAddress,
			.operation = MemoryOperation::Write,
			.get_next_used_idx = originalInstruction.get_next_used_idx
		};

		next->ProcessRequest(writeBackInstruction);
	}
}

std::string Cache::ToString()
{
	std::string str = "";
	std::stringstream ss;

	ss << "===== " << name << " contents =====\n";
	str.append(ss.str());
	ss.str("");
	ss.clear();

	SetIndex set;
	AssociativityIndex associativityIdx;
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

std::string Cache::StatisticsOutput(char startingLineIdentifier = 'a')
{
	std::string str = "";
	std::stringstream ss;
	
	char currentLineIdentifier = startingLineIdentifier;

	ss << currentLineIdentifier++ << ". number of " << Cache::name << " reads:        " << Cache::statistics.reads << std::endl;
	ss << currentLineIdentifier++ << ". number of " << Cache::name << " read misses:  " << Cache::statistics.readMisses << std::endl;
	ss << currentLineIdentifier++ << ". number of " << Cache::name << " writes:       " << Cache::statistics.writes << std::endl;
	ss << currentLineIdentifier++ << ". number of " << Cache::name << " write misses: " << Cache::statistics.writeMisses << std::endl;

	double missRate;
	// miss rates that stall the CPU
	if (!prev || next)
		missRate = (double)(Cache::statistics.readMisses + Cache::statistics.writeMisses) / (double)(Cache::statistics.reads + Cache::statistics.writes);
	// Miss rate for the lowest cache level
	else
		missRate = (double)(Cache::statistics.readMisses) / (double)(Cache::statistics.reads);
	
	ss << currentLineIdentifier++ << ". " << Cache::name << " miss rate:              " << std::fixed << std::setprecision(6) << missRate << std::endl;

	ss << currentLineIdentifier++ << ". number of " << Cache::name << " writebacks:   " << Cache::statistics.writeBacks << std::endl;

	str.append(ss.str());
	ss.str("");
	ss.clear();

	return str;
}

void Cache::Evict(Address address)
{
	// Determine the set the instruction belongs to
	SetIndex set = GetSet(address);

	int i;
	for (i = 0; i < Cache::associativity; i++)
	{
		// Check if the current address block is hit
		if (Cache::ToTag(address) == Cache::memory[set][i].tag)
		{
			// Propogate dirty bits to main memory
			if (Cache::memory[set][i].isDirty && next)
				next->PropagateWriteToMainMemory();

			// Reset line of cache memory
			Cache::memory[set][i] = CacheLine();
			// Reset Replacement Block data
			switch (Cache::replacement)
			{
				case ReplacementPolicy::FIFO:
				case ReplacementPolicy::LRU:
					Cache::replacementData[set][i] = 0;
					break;
			}
			
			// Cascade evictions to upper levels of cache for inclusive policy
			if (Cache::isInclusive && prev)
				prev->Evict(address);

			return;
		}
	}

	// For evicting address in higher levels of cache for inclusive memory hierarchies,
	// this return should not be reachable. 
	return;
}

void Cache::PropagateWriteToMainMemory()
{
	// Update statistics
	Cache::statistics.writePropagations++;

	// Continue propogation
	if (next)
		next->PropagateWriteToMainMemory();
}
