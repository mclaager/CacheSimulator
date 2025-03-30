#ifndef CACHESTATISTICS_H
#define CACHESTATISTICS_H

struct CacheStatistics
{
    unsigned int reads = 0;
    unsigned int readMisses = 0;

    unsigned int writes = 0;
    unsigned int writeMisses = 0;

    // Cache miss rate = (readMisses + writeMisses) / (reads + writes)

    unsigned int writeBacks = 0;
};

#endif