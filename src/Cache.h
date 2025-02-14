#ifndef CACHE_H
#define CACHE_H

#include "ICache.h"

class Cache : public ICache
{
public:
    int size, associativity, blockSize;

    Cache(int size, int associativity, int blockSize);

    bool ProcessRequest(Instruction instruction) override;
    void Evict(Address address) override;
    void Issue(Address address) override;
};

#endif
