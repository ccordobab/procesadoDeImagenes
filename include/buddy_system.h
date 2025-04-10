#ifndef BUDDY_SYSTEM_H
#define BUDDY_SYSTEM_H

#include <vector>
#include <cstddef>

struct BuddyBlock {
    size_t size;
    bool isFree;
    size_t offset;
    void* ptr;
};

class BuddySystem {
private:
    size_t totalSize;
    std::vector<BuddyBlock> blocks;
    std::vector<void*> allocatedPointers;

    size_t nextPowerOfTwo(size_t size);
    void splitBlock(size_t index, size_t size);
    void mergeBlocks();

public:
    explicit BuddySystem(size_t totalSize);
    ~BuddySystem();
    
    void* allocate(size_t size);
    void free(void* ptr);
    void printMemoryStatus() const;
};

#endif