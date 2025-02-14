#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <memory>

#include "FileProcessor.h"
#include "Cache.h"

#include "MemoryHierarchy.h"

/// <summary>
/// The simulator accepts 8 Command Line args, the following are:
/// BLOCKSIZE: Positive integer. Block size in bytes. (Same block size for all 
/// 	caches in the memory hierarchy.) 
/// L1_SIZE: Positive integer. L1 cache size in bytes. 
/// L1_ASSOC: Positive integer. L1 set-associa vity (1 is direct-mapped). 
/// L2_SIZE: Positive integer. L2 cache size in bytes. L2_SIZE = 0 signifies 
/// 	that there is no L2 cache. 
/// L2_ASSOC: Positive integer. L2 set-associa vity (1 is direct-mapped). 
/// REPLACEMENT_POLICY: Posi ve integer. 0 for LRU, 1 for FIFO, 2 
/// 	for opmal. 
/// INCLUSION_PROPERTY: Positive integer. 0 for non-inclusive, 1 for inclusive. 
/// trace_file: Character string. Full name of trace file including any extensions. 
/// </summary>
int main(int argc, char** argv)
{
	int blockSize,
		l1_size, l1_assoc,
		l2_size, l2_assoc,
		replacementPolicy, isInclusive;

	std::string traceFile; 
	
	if (argc < 8)
	{
		std::cerr << "Error: Not enough arguments." << std::endl;
		return EXIT_FAILURE;
	}

	blockSize = atoi(argv[1]);
	l1_size = atoi(argv[2]);
	l1_assoc = atoi(argv[3]);
	l2_size = atoi(argv[4]);
	l2_assoc = atoi(argv[5]);
	replacementPolicy = atoi(argv[6]);
	isInclusive = atoi(argv[7]);
	traceFile = argv[8];
	
	Cache cacheL1 = Cache(l1_size, l1_assoc, blockSize);
	Cache cacheL2 = Cache(l2_size, l2_assoc, blockSize);

	std::vector<std::shared_ptr<ICache>> caches;
	caches.push_back(std::make_shared<Cache>(cacheL1));
	caches.push_back(std::make_shared<Cache>(cacheL2));

	MemoryHierarchy mh = MemoryHierarchy(caches);

	std::cout << (dynamic_cast<Cache*>(mh.cacheModules[0].get()))->size << std::endl;

    std::string fileloc = traceFile;
    FileProcessor processor = FileProcessor(fileloc);

    Instruction instruct = processor.Next();

    std::cout << instruct.operation << std::endl;
    std::cout << instruct.address << std::endl;

    instruct = processor.Next();

    std::cout << instruct.operation << std::endl;
    std::cout << instruct.address << std::endl;

    instruct = processor.Next();

    std::cout << instruct.operation << std::endl;
    std::cout << instruct.address << std::endl;

    instruct = processor.Next();

    std::cout << instruct.operation << std::endl;
    std::cout << instruct.address << std::endl;

    instruct = processor.Next();

    std::cout << instruct.operation << std::endl;
    std::cout << instruct.address << std::endl;
}
