# Cache Simulator

A C++17 L1 cache simulator with parameterized capacity, associativity, and block size. The simulator uses strict LRU replacement per set and reports total accesses, hits, misses, and hit rate.

## Features

- N-way set-associative cache model
- LRU replacement implemented with `std::list` and `std::unordered_map`
- Dynamic bit-width calculation for index and block offset fields
- Bitwise tag/index extraction from 32-bit addresses
- Trace-file driven execution

## Repository Layout

- `includes/cache.h` - cache interface and set/LRU definitions
- `src/cache.cpp` - cache constructor, access logic, and statistics
- `src/main.cpp` - trace parser and simulator entry point
- `traces/` - place input trace files here
- `Makefile` - build recipe for the simulator

## Build

Prerequisites:

- `g++` with C++17 support
- `make`

Build the simulator from the repository root:

```bash
make
```

This produces the executable `simulator` in the repository root.

To remove build outputs:

```bash
make clean
```

## Run

The program expects a trace file path as its first argument:

```bash
./simulator traces/sample.trace
```

On Linux, the Makefile builds `simulator` (not `simulator.exe`).

Each trace line must contain an access type and a hexadecimal 32-bit address:

```text
R 0x1A2B3C4D
W 0xFFEEDDCC
```

- `R` means read
- `W` means write
- The address is parsed as base-16 with `std::stoul`

## Cache Configuration

The current executable uses these defaults in `src/main.cpp`:

- Cache size: 32768 bytes
- Associativity: 4-way
- Block size: 64 bytes

From these values, the simulator derives:

- Number of sets = `cacheSize / (associativity * blockSize)`
- Offset bits = `log2(blockSize)`
- Index bits = `log2(number of sets)`
- Tag bits = `32 - indexBits - offsetBits`

## Reproducibility Notes

The following command sequence reproduces the current build and execution flow from a clean checkout:

```bash
make clean
make
./simulator traces/<your-trace-file>
```

The simulator prints:

- total accesses
- hits
- misses
- hit rate

## Expected Behavior

The cache uses strict LRU replacement per set:

- On a hit, the accessed tag moves to the front of the set’s LRU list
- On a miss, the new tag is inserted at the front
- If the set is full, the least recently used tag at the back is evicted first

## Notes

- Trace parsing ignores blank lines and malformed lines
- The simulator assumes addresses fit within 32 bits
- `main.cpp` currently uses fixed cache parameters; change those constants if you want to experiment with other configurations
