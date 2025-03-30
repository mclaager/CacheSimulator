#include <iostream>

#include "OracleFileProcessor.h"

OracleFileProcessor::OracleFileProcessor(const std::string& file, unsigned int blockSize) : IFileProcessor(file), stream(file), blockSize(blockSize)
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

    unsigned int traceIdx = 0;
    Address address = 0;
    Block block = 0;
    std::string str;
    while (!Finished())
    {
        if (std::getline(stream, str) && !str.empty())
        {
            try
            {
                if (str.size() > 2)
                    address = std::stoul(str.substr(2), nullptr, 16);
                
                block = ToBlock(address);
                
                // First appearance of address
                if (reuseIdxs.count(block) == 0)
                    reuseIdxs[block] = std::list<unsigned int>({ traceIdx });
                // Subsequent appearances
                else
                    reuseIdxs[block].push_back(traceIdx);
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error parsing address from line: " << str << "\n";
            }
        }

        traceIdx++;
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
    Block block = 0;

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
            
            block = ToBlock(address);
            
            if (reuseIdxs.count(block) == 0)
                throw;
            
            reuseIdxs[block].pop_front();
            
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing address from line: " << str << "\n";
        }
    }

    retrn = 
    {
        .address = address,
        .operation = op,
        .get_next_used_idx = [&](Address address){ return OracleFileProcessor::GetNextUsedTraceIndex(address); }
    };

    instructionCounter++;

    // DebugPrintOracleMemory();
    // std::cout << "Cycles Until Reuse: " << retrn.cyclesUntilReuse << std::endl;

    return retrn;
}

unsigned int OracleFileProcessor::GetNextUsedTraceIndex(Address address)
{
    Block block = ToBlock(address);
    
    if (reuseIdxs[block].size() == 0)
        return NEVER_REUSED;
    else
        return reuseIdxs[block].front();
}

Block OracleFileProcessor::ToBlock(Address address)
{
    return address / OracleFileProcessor::blockSize;
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