#ifndef CACHESTATUS_H
#define CACHESTATUS_H

#include "Instruction.h"

// Descriptors giving an overview of the success for a cache request
enum CacheStatus
{
    CacheHit,
    CacheMissNoEviction,
    CacheMissEviction,
};

// The output for any request that is processed by a cache module
struct CacheRequestOutput
{
    // The overall success of the cache's module rqeuest.
    CacheStatus status;

    // A memory address that may need to be known the memory hierarchy
    // (i.e. on cache miss with eviction)
    Address address;
};


#endif