#include <iostream>

#include "OracleFileProcessor.h"

OracleFileProcessor::OracleFileProcessor(const std::string& file) : IFileProcessor(file), stream(file)
{
    if (!stream)
    {
        std::cerr << "Error: Failed to open file: " << file << std::endl;
    }

    OracleFileProcessor::instructionCounter = 0;

    PreProcessTrace();
}

void OracleFileProcessor::PreProcessTrace()
{
    if (!stream.is_open())
    {
        std::cerr << "Error: File failed to open.\n";
    }

    unsigned int addressIdx = 0;
    Address address = 0;
    std::string str;
    while (!Finished())
    {
        if (std::getline(stream, str) && !str.empty())
        {
            try
            {
                if (str.size() > 2)
                    address = std::stoul(str.substr(2), nullptr, 16);
                
                // First appearance of address
                if (reuseIdxs.count(address) == 0)
                    reuseIdxs[address] = std::list<unsigned int>({ addressIdx });
                // Subsequent appearances
                else
                    reuseIdxs[address].push_back(addressIdx);
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error parsing address from line: " << str << "\n";
            }
        }

        addressIdx++;
    }

    // DebugPrintOracleMemory();

    // Reset stream back to beginning
    stream.clear();
    stream.seekg(0, std::ios::beg);
}

// Reached the end of the file
bool OracleFileProcessor::Finished()
{
    return stream.eof();
}

// Grab the next instruction
Instruction OracleFileProcessor::Next()
{
    Instruction retrn;
    MemoryOperation op = None;
    Address address = 0;
    unsigned int reuseIdx = 0;

    if (!stream.is_open())
    {
        std::cerr << "Error: File failed to open.\n";
    }

    std::string str;
    if (std::getline(stream, str) && !str.empty())
    {
        if (str[0] == 'r')
            op = Read;
        else if (str[0] == 'w')
            op = Write;

        try
        {
            if (str.size() > 2)
                address = std::stoul(str.substr(2), nullptr, 16);
            
            if (reuseIdxs.count(address) == 0)
                throw;
            
            reuseIdxs[address].pop_front();
            if (reuseIdxs[address].size() == 0)
                reuseIdx = NEVER_REUSED;
            else
                reuseIdx = reuseIdxs[address].front();
            
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing address from line: " << str << "\n";
        }
    }

    retrn = Instruction();
    retrn.address = address;
    retrn.operation = op;
    retrn.cyclesUntilReuse = reuseIdx == NEVER_REUSED ?
        NEVER_REUSED :
        reuseIdx - OracleFileProcessor::instructionCounter;

    instructionCounter++;

    // DebugPrintOracleMemory();
    // std::cout << "Cycles Until Reuse: " << retrn.cyclesUntilReuse << std::endl;

    return retrn;
}

void OracleFileProcessor::DebugPrintOracleMemory()
{
    for (auto kvp : OracleFileProcessor::reuseIdxs)
    {
        std::cout << "Address: " << kvp.first << std::endl << "Access Indices: ";
        for (auto idx : kvp.second)
        {
            std::cout << idx << " ";
        }
        std::cout << std::endl;
    }
}