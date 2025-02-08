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
    virtual bool Finished() = 0;
    // Grab the next instruction
    virtual Instruction Next() = 0;
};

#endif