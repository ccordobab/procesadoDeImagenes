#include "buddy_system.h"
#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>

BuddySystem::BuddySystem(size_t totalSize) : totalSize(nextPowerOfTwo(totalSize)) {
    blocks.push_back({this->totalSize, true, 0, nullptr});
    std::cout << "Buddy System initialized with " << this->totalSize << " bytes\n";
}

BuddySystem::~BuddySystem() {
    // Liberar toda la memoria asignada
    for (void* ptr : allocatedPointers) {
        ::free(ptr);
        std::cout << "Automatically freed memory at " << ptr << "\n";
    }
    allocatedPointers.clear();
    blocks.clear();
    std::cout << "Buddy System destroyed\n";
}

size_t BuddySystem::nextPowerOfTwo(size_t size) {
    if (size == 0) return 1;
    size_t power = 1;
    while (power < size) power <<= 1;
    return power;
}

void BuddySystem::splitBlock(size_t index, size_t size) {
    while (blocks[index].size / 2 >= size) {
        size_t newSize = blocks[index].size / 2;
        size_t offset = blocks[index].offset;

        BuddyBlock newBlock = {newSize, true, offset + newSize, nullptr};
        blocks[index].size = newSize;
        blocks.insert(blocks.begin() + index + 1, newBlock);
        
        std::cout << "Split block at offset " << offset 
                  << " into two blocks of " << newSize << " bytes\n";
    }
}

void* BuddySystem::allocate(size_t size) {
    if (size == 0) {
        std::cerr << "Allocation failed: Cannot allocate 0 bytes\n";
        return nullptr;
    }
    
    size = nextPowerOfTwo(size);
    if (size > totalSize) {
        std::cerr << "Allocation failed: Requested size (" << size 
                  << " bytes) exceeds total memory (" << totalSize << " bytes)\n";
        return nullptr;
    }

    for (size_t i = 0; i < blocks.size(); ++i) {
        if (blocks[i].isFree && blocks[i].size >= size) {
            splitBlock(i, size);
            blocks[i].isFree = false;
            
            void* ptr = malloc(size);
            if (!ptr) {
                std::cerr << "Allocation failed: System out of memory\n";
                return nullptr;
            }
            
            blocks[i].ptr = ptr;
            allocatedPointers.push_back(ptr);

            std::cout << "Allocated " << size << " bytes at offset " << blocks[i].offset 
                      << " (ptr: " << ptr << ")\n";
            return ptr;
        }
    }

    std::cerr << "Allocation failed: Not enough contiguous memory for " 
              << size << " bytes\n";
    printMemoryStatus();
    return nullptr;
}

void BuddySystem::free(void* ptr) {
    if (!ptr) {
        std::cerr << "Free failed: Null pointer\n";
        return;
    }

    auto it = std::find(allocatedPointers.begin(), allocatedPointers.end(), ptr);
    if (it == allocatedPointers.end()) {
        std::cerr << "Free failed: Pointer " << ptr << " not found in allocated memory\n";
        return;
    }

    // Encontrar y marcar el bloque correspondiente como libre
    bool blockFound = false;
    for (auto& block : blocks) {
        if (block.ptr == ptr) {
            ::free(ptr);
            block.isFree = true;
            block.ptr = nullptr;
            blockFound = true;
            break;
        }
    }

    if (!blockFound) {
        std::cerr << "Warning: Pointer " << ptr << " found in allocated list but not in blocks\n";
    }

    allocatedPointers.erase(it);
    mergeBlocks();
    std::cout << "Successfully freed memory at " << ptr << "\n";
}

void BuddySystem::mergeBlocks() {
    bool merged;
    do {
        merged = false;
        for (size_t i = 0; i < blocks.size() - 1; ++i) {
            BuddyBlock& current = blocks[i];
            BuddyBlock& next = blocks[i+1];
            
            if (current.isFree && next.isFree && 
                current.size == next.size &&
                current.offset + current.size == next.offset) {
                
                current.size *= 2;
                blocks.erase(blocks.begin() + i + 1);
                merged = true;
                
                std::cout << "Merged blocks at offset " << current.offset 
                          << " and " << next.offset << " into " 
                          << current.size << " bytes\n";
                break;
            }
        }
    } while (merged);
}

void BuddySystem::printMemoryStatus() const {
    std::cout << "\n=== Memory Status ===\n";
    std::cout << "Total size: " << totalSize << " bytes\n";
    std::cout << "Allocated blocks: " << allocatedPointers.size() << "\n";
    std::cout << "Memory blocks:\n";
    
    for (const auto& block : blocks) {
        std::cout << "  Offset: " << block.offset
                  << " | Size: " << block.size << " bytes"
                  << " | Status: " << (block.isFree ? "Free" : "Used")
                  << " | Ptr: " << block.ptr << "\n";
    }
    std::cout << "====================\n";
}