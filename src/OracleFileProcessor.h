#ifndef ORACLEFILEPROCESSOR_H
#define ORACLEFILEPROCESSOR_H

#include <fstream>
#include <string>
#include <unordered_map>
#include <list>

#include "IFileProcessor.h"
#include "types/Instruction.h"

class OracleFileProcessor : public IFileProcessor
{
private:
    std::ifstream stream;
    unsigned int instructionCounter;
    unsigned int blockSize;
    std::unordered_map<Address, std::list<unsigned int>> reuseIdxs;
    OracleFileProcessor();

    // Find reuse distances for each instruction
    void PreProcessTrace();

    Block ToBlock(Address address);

    void DebugPrintOracleMemory();
public:
    OracleFileProcessor(const std::string& file, unsigned int blockSize);

    // Reached the end of the file
    bool Finished() override;

    // Grab the next instruction
    Instruction Next() override;

    // Get the trace index where the block is next used
    unsigned int GetNextUsedTraceIndex(Block block);
};

#endif