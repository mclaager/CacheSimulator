#include "Statistics.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

StatisticsOutput::StatisticsOutput(const std::string& basefilename) 
    : basefilename(basefilename), fileCount(1) {
    lastLineArbiter = "end";  // Sentinel value
    MissPenalty = 0.000000100; // 100ns
}

void StatisticsOutput::writeStatisticsOutput(const std::string& data)
{
    // If appending is not allowed, increment the file count
    while (!isAppendingAllowed())
    {
        fileCount++;  // Increment to the next file if sentinel value is found
    }

    // Open the current file in append mode
    std::ofstream outFile(getCurrentFileName(), std::ios::app);
    if (outFile.is_open())
    {
        outFile << data << std::endl;  // Write the data
        outFile.close();  // Close the file after writing
    }
    else
    {
        std::cerr << "Unable to open file: " << getCurrentFileName() << std::endl;
    }
}

bool StatisticsOutput::isAppendingAllowed()
{
    std::ifstream inFile(getCurrentFileName());
    std::string lastLine;

    // Check if the file is empty or has the sentinel value
    if (inFile.peek() == std::ifstream::traits_type::eof()) {
        return true;  // File is empty, allow appending
    }

    // Read the last line of the file
    while (std::getline(inFile, lastLine));  // Loop until last line

    // If the last line is the sentinel value, disallow appending
    return lastLine != lastLineArbiter;
}

std::string StatisticsOutput::getCurrentFileName()
{
    std::stringstream ss;
    ss << basefilename << fileCount << ".txt";  // Construct filename
    return ss.str();
}

// Total access time no L2
    // TATsL2
    /*
        (L1_reads + l1_writes)*HitTime + (L1_read_misses + L1_write_misses) * MissPenalty
    */
double StatisticsOutput::calcTotalAccessTimeNoL2( int L1R, int L1RM, int L1W, int L1WM )
{
    return double((L1R + L1W) + )
}

// Total access time with L2
