#include <iostream>

#include "FileProcessor.h"

FileProcessor::FileProcessor(const std::string& file) : IFileProcessor(file), stream(file)
{
    if (!stream)
    {
        std::cerr << "Error: Failed to open file: " << file << std::endl;
    }
}

// Reached the end of the file
bool FileProcessor::Finished()
{
    return stream.eof();
}

// Grab the next instruction
Instruction FileProcessor::Next()
{
    Instruction retrn;
    MemoryOperation op = None;
    Address address = 0;

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
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing address from line: " << str << "\n";
        }
    }

    retrn = Instruction();
    retrn.address = address;
    retrn.operation = op;
    retrn.cyclesUntilReuse = REUSE_NOT_APPLICABLE;

    return retrn;
}