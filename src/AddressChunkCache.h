#ifndef ADDRESSCHUNKCACHE_H
#define ADDRESSCHUNKCACHE_H

#include "Cache.h"

class AddressChunkCache : public Cache
{
public:
    int n, m;
    unsigned int currentAddressHeaderIdx;
    unsigned int missAddressChunkIdx;
    unsigned int currentReplacementIdx;
    std::vector<std::vector<Address>> addressChunks;
    AddressChunkCache(int size, int associativity, int blockSize, int m, int n, ReplacementPolicy replacement, std::string name);

    CacheRequestOutput ProcessRequest(Instruction instruction) override;
    void Evict(Address address) override;

    std::string ToString() override;
};

#endif