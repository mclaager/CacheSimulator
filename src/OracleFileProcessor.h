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
    std::unordered_map<Address, std::list<unsigned int>> reuseIdxs;
    OracleFileProcessor();

    // Find reuse distances for each instruction
    void PreProcessTrace();

    void DebugPrintOracleMemory();
public:
    OracleFileProcessor(const std::string& file);

    // Reached the end of the file
    bool Finished() override;

    // Grab the next instruction
    Instruction Next() override;

    unsigned int GetReuseDistance(Address address);
};

#endif