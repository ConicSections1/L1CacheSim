#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "cache.h"

int main(int argc, char* argv[]) {
    std::cout << "\n--- RISC-V Architectural Cache Simulator ---\n";

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <trace_file>\n";
        return 1;
    }

    // Hardcoded defaults for the L1 cache model.
    int cacheSize = 32768; // 32KB
    int associativity = 4; // 4-way
    int blockSize = 64;    // 64 Bytes

    Cache l1_cache(cacheSize, associativity, blockSize);

    std::ifstream traceFile(argv[1]);
    if (!traceFile.is_open()) {
        std::cerr << "Failed to open trace file: " << argv[1] << "\n";
        return 1;
    }

    std::string line;
    while (std::getline(traceFile, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        char operation = '\0';
        std::string addressText;

        if (!(iss >> operation >> addressText)) {
            continue;
        }

        uint32_t address = static_cast<uint32_t>(std::stoul(addressText, nullptr, 16));
        l1_cache.accessMemory(address, operation);
    }

    l1_cache.printStats();
    
    std::cout << "--------------------------------------------\n\n";

    return 0;
}
