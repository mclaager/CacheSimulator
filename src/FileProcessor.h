#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#include <fstream>
#include <string>

#include "IFileProcessor.h"
#include "types/Instruction.h"

class FileProcessor : IFileProcessor
{
private:
    std::ifstream stream;
    FileProcessor();
public:
    FileProcessor(const std::string& file);

    // Reached the end of the file
    bool Finished() override;

    // Grab the next instruction
    Instruction Next() override;
};

#endif