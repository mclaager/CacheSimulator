#ifndef IFILEPROCESSOR_H
#define IFILEPROCESSOR_H

#include <string>

#include "types/Instruction.h"

class IFileProcessor
{
private:
    std::string file;
    IFileProcessor();
public:
    IFileProcessor(const std::string &file) : file(file) { }
    
    // Reached the end of the file
    bool Finished();
    // Grab the next instruction
    Instruction Next();
};

#endif