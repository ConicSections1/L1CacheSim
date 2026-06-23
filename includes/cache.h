#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

class Cache {
public:
    // Constructor
    Cache(int size, int assoc, int blockSize);
    
    // Core function we will build out later
    void accessMemory(uint32_t address, char operation);
    
    // Output metrics
    void printStats() const;

private:
    struct CacheSet {
        std::list<uint32_t> lruOrder;
        std::unordered_map<uint32_t, std::list<uint32_t>::iterator> tagLookup;
    };

    int cacheSize;
    int associativity;
    int blockSize;
    int numSets;
    int indexBits;
    int offsetBits;
    int hits;
    int misses;
    int accesses;

    std::vector<CacheSet> cacheSets;
};

#endif
