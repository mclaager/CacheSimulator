#include "AddressChunkCache.h"

#include <sstream>

AddressChunkCache::AddressChunkCache(int size, int associativity, int blockSize, int n, int m, ReplacementPolicy replacement, std::string name)
	: Cache(size, associativity, blockSize, replacement, name), n(n), m(m)
{
    AddressChunkCache::addressChunks.resize(n, std::vector<Address>(m, 0));
    AddressChunkCache::currentAddressHeaderIdx = 0;
    AddressChunkCache::missAddressChunkIdx = 0;
    AddressChunkCache::currentReplacementIdx = 0;
}

CacheRequestOutput AddressChunkCache::ProcessRequest(Instruction instruction)
{
    CacheRequestOutput baseOutput = Cache::ProcessRequest(instruction);
    if (baseOutput.status == CacheHit || n == 0 || m == 0)
        return baseOutput;

    CacheRequestOutput output;

    // Step 1: Check for cache hit
    int i;
    for (i = 0; i < AddressChunkCache::n; i++)
    {
        if (addressChunks[i][0] == instruction.address / Cache::blockSize)
        {
            AddressChunkCache::currentAddressHeaderIdx = i;
            switch (instruction.operation)
            {
                case Read: Cache::statistics.readMisses--; break;
                case Write: Cache::statistics.writeMisses--; break;
            }
            output = 
            {
                .status = CacheHit,
                .address = addressChunks[i][0]
            };
            return output;
        }
    }

    // Step 2: Use the current address header to search for other entries
    for (i = 1; i < AddressChunkCache::m + 1; i++)
    {
        if (addressChunks[AddressChunkCache::currentAddressHeaderIdx][i] == instruction.address / Cache::blockSize)
        {
            switch (instruction.operation)
            {
                case Read: Cache::statistics.readMisses--; break;
                case Write: Cache::statistics.writeMisses--; break;
            }
            output = 
            {
                .status = CacheHit,
                .address = addressChunks[AddressChunkCache::currentAddressHeaderIdx][i]
            };
            return output;
        }
    }

    // Step 3: Cache Miss, replacement for address chunk
    // TODO: Currently only implemented on FIFO, implement other replacement policies
    // TODO: Does not support inclusivity
    AddressChunkCache::addressChunks[AddressChunkCache::missAddressChunkIdx][AddressChunkCache::currentReplacementIdx++] = instruction.address / Cache::blockSize;

    if (AddressChunkCache::currentReplacementIdx > m)
    {
        AddressChunkCache::currentReplacementIdx = 0;
        AddressChunkCache::missAddressChunkIdx = (AddressChunkCache::missAddressChunkIdx + 1) % n;
    }

    output = 
    {
        .status = CacheMissNoEviction,
        .address = instruction.address
    };
    return output;
}

void AddressChunkCache::Evict(Address address)
{
    Cache::Evict(address);

    // TODO: How / can this be optimized?
    int i, j;
    for (i = 0; i < AddressChunkCache::n; i++)
    {
        for (int j = 1; j < AddressChunkCache::m + 1; j++)
        {
            if (address == AddressChunkCache::addressChunks[i][j])
            {
                // TODO: Set to proper invalid
                AddressChunkCache::addressChunks[i][j] = 0;
            }
        }
    }
}

std::string AddressChunkCache::ToString()
{
    std::string output = Cache::ToString();

    std::stringstream ss;

	output.append("----- Address Chunks -----\n");
    output.append("Address Chunk Headers     Addresses\n");

    int i, j;
    for (i = 0; i < AddressChunkCache::n; i++)
    {
        ss << "Chunk " << i << ": [" << AddressChunkCache::addressChunks[i][0] << "]    ";
        output.append(ss.str());
        ss.str("");
	    ss.clear();

        for (j = 1; j < AddressChunkCache::m + 1; j++)
        {
            ss << " " << AddressChunkCache::addressChunks[i][j];
            output.append(ss.str());
            ss.str("");
            ss.clear();
        }

        output.append("\n");
    }

    return output;
}