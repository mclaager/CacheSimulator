#include <cstdio>
#include <iostream>

#include "FileProcessor.h"

#include "MemoryHierarchy.h"

int main(int argc, char** argv)
{
    std::string fileloc = "../../../data/traces/go_trace.txt";
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