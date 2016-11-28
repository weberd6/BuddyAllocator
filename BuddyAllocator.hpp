#ifndef BUDDY_ALLOCATOR_HPP
#define BUDDY_ALLOCATOR_HPP

#include <vector>
#include <unordered_map>

class BuddyAllocator {
public:

    BuddyAllocator() = default;

    void initialize(unsigned int max_order, unsigned int min_order = 0);
    void* allocate(std::size_t n);
    void deallocate(void* p);

    void printFreeLists();

    struct Block {
        Block(Block* n, unsigned int o) : next(n), order(o) {}
        Block* next;
        unsigned int order;
    };

protected:

    unsigned int nextPower2(unsigned int v);
    Block* getBuddy(Block* b, unsigned int order);

    unsigned long blockId(Block* b);
    void setAllocated(unsigned long b);
    void clearAllocated(unsigned long b);

    bool isFree(Block* b);
    bool listEmpty(unsigned int order);
    void listAdd(Block* b);
    void listRemove(Block* b);
    Block* listRemove(unsigned int order);

private:

    unsigned int max_order_;
    unsigned int min_order_;

    void* base_address_;
    Block** free_list_by_order_;
    std::vector<bool> tag_by_blockid_;

    std::unordered_map<void*, unsigned int> size_by_address_;
};

#endif
