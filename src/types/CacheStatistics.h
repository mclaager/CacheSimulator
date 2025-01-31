#ifndef CACHESTATISTICS_H
#define CACHESTATISTICS_H

struct CacheStatistics
{
    int reads;
    int readMisses;

    int writes;
    int writeMisses;

    // Cache miss rate = (readMisses + writeMisses) / (reads + writes)
};

#endif