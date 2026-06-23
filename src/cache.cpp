#include "cache.h"

#include <cmath>
#include <iomanip>

Cache::Cache(int size, int assoc, int blockSize)
    : cacheSize(size), associativity(assoc), blockSize(blockSize),
      numSets(size / (assoc * blockSize)),
      indexBits(static_cast<int>(std::log2(size / (assoc * blockSize)))),
      offsetBits(static_cast<int>(std::log2(blockSize))),
      hits(0), misses(0), accesses(0), cacheSets(numSets) {
    std::cout << "Cache configured: " << size << "B, "
              << assoc << "-way associative, " << blockSize << "B blocks.\n";
}

void Cache::accessMemory(uint32_t address, char operation) {
    accesses++;

    const uint32_t indexMask = (indexBits == 0) ? 0u : ((1u << indexBits) - 1u);
    const uint32_t index = (address >> offsetBits) & indexMask;
    const uint32_t tag = address >> (offsetBits + indexBits);

    CacheSet &set = cacheSets[index];
    const auto found = set.tagLookup.find(tag);

    if (found != set.tagLookup.end()) {
        hits++;
        set.lruOrder.splice(set.lruOrder.begin(), set.lruOrder, found->second);
        found->second = set.lruOrder.begin();
        return;
    }

    misses++;

    if (static_cast<int>(set.lruOrder.size()) == associativity) {
        const uint32_t evictedTag = set.lruOrder.back();
        set.tagLookup.erase(evictedTag);
        set.lruOrder.pop_back();
    }

    set.lruOrder.push_front(tag);
    set.tagLookup[tag] = set.lruOrder.begin();

    (void)operation;
}

void Cache::printStats() const {
    std::cout << "Total Accesses: " << accesses << "\n";
    std::cout << "Hits: " << hits << " | Misses: " << misses << "\n";
    const double hitRate = accesses == 0 ? 0.0 : static_cast<double>(hits) / static_cast<double>(accesses);
    std::cout << std::fixed << std::setprecision(4)
              << "Hit Rate: " << hitRate * 100.0 << "%\n";
}
