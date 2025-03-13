#ifndef STATISTICS_H
#define STATISTICS_H

#include <string>

class StatisticsOutput
{
public:
    // Constructor with filename base
    StatisticsOutput(const std::string& basefilename);

    // Method to write statistics to the current file
    void writeStatisticsOutput(const std::string& data);

    double calcTotalAccessTimeNoL2( int L1R, int L1RM, int L1W, int L1WM );

    double clacTotalAccessTimeWL2( int L1R, int L1RM, int L1W, int L1WM );

private:
    std::string basefilename;   // Base name for the file
    int fileCount;              // Counter for generating file names
    double MissPenalty;
    std::string lastLineArbiter; // Sentinel value

    // Check if appending is allowed (based on sentinel value)
    bool isAppendingAllowed();

    // Generate the current file name based on basefilename and fileCount
    std::string getCurrentFileName();
};

#endif
