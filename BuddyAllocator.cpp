#include "BuddyAllocator.hpp"

#include <cmath>
#include <sstream>
#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <atomic>
#include <unordered_map>

void BuddyAllocator::initialize(unsigned int max_order, unsigned int min_order) {

    max_order_ = max_order;
    min_order_ = std::max(min_order, (unsigned int)std::log2(nextPower2(sizeof(Block))));

    free_list_by_order_ = new Block*[max_order+1];

    for (unsigned int i = 0; i < max_order_; i++) {
        free_list_by_order_[i] = nullptr;
    }

    base_address_ = malloc(1 << max_order);
    free_list_by_order_[max_order_] = (Block*)base_address_;

    unsigned long max_blocks = (1 << max_order_) / (1 << min_order_);
    tag_by_blockid_.assign(max_blocks, false);
}

void BuddyAllocator::setAllocated(unsigned long b) {
    tag_by_blockid_[b] = true;
}

void BuddyAllocator::clearAllocated(unsigned long b) {
    tag_by_blockid_[b] = false;
}

unsigned int BuddyAllocator::nextPower2(unsigned int v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

BuddyAllocator::Block* BuddyAllocator::getBuddy(Block* b, unsigned int order) {
    uintptr_t block = uintptr_t(b) - (uintptr_t)base_address_;
    uintptr_t buddy = block ^ (uintptr_t)(1 << order);
    return (Block*)((uintptr_t)base_address_ + buddy);
}

unsigned long BuddyAllocator::blockId(Block* b) {
    return ((uintptr_t)b - (uintptr_t)base_address_) >> min_order_;
}

bool BuddyAllocator::isFree(Block* b) {
    unsigned long block_id = blockId(b);
    return tag_by_blockid_[block_id] == false;
}

bool BuddyAllocator::listEmpty(unsigned int order) {
    return (free_list_by_order_[order] == nullptr);
}

void BuddyAllocator::listAdd(Block* b) {
    Block* p = free_list_by_order_[b->order];
    free_list_by_order_[b->order] = b;
    b->next = p;
}

void BuddyAllocator::listRemove(Block* b) {

    Block* p = free_list_by_order_[b->order];
    if (p == b) {
        free_list_by_order_[b->order] = p->next;
        return;
    }

    Block* n = p->next;
    while (n != b) {
        p = p->next;
        assert(p != nullptr);
        n = p->next;
    }
    p->next = n->next;
}

BuddyAllocator::Block* BuddyAllocator::listRemove(unsigned int order) {
    Block* p = free_list_by_order_[order];
    free_list_by_order_[order] = p->next;
    return p;
}

void* BuddyAllocator::allocate(std::size_t n) {

    unsigned int order = std::max((unsigned int)std::log2(nextPower2(n)), min_order_);
    Block* b = nullptr;
    Block* bb = nullptr;

    unsigned int i;
    for (i = order; i <= max_order_; i++) {
        if (listEmpty(i))
            continue;

        b = listRemove(i);
        setAllocated(blockId(b));

        while (i > order) {
            --i;
            bb = getBuddy(b, i);
            bb->order = i;
            clearAllocated(blockId(bb));
            listAdd(bb);
        }

        size_by_address_[b] = n;
        return b;
    }

    throw std::runtime_error("Out of memory");
    return nullptr;
}

void BuddyAllocator::deallocate(void* p) {

    Block* b = (Block*)p;

    unsigned int n = size_by_address_[p];
    unsigned int order = std::max((unsigned int)std::log2(nextPower2(n)), min_order_);

    if (isFree(b)) {
        std::stringstream stream;
        stream << std::hex << p;
        std::string address(stream.str());
        throw std::runtime_error("Invalid Free: "+address);
    }

    while (order < max_order_) {
        Block* bb = getBuddy(b, order);

        if ((!isFree(bb)) ||
            ((isFree(bb) && (order != bb->order)))) {
            break;
        }

        listRemove(bb);
        b = (Block*)std::min((uintptr_t)b, (uintptr_t)bb);
        b->order = ++order;
    }

    b->order = order;
    clearAllocated(blockId(b));
    listAdd(b);
}

void BuddyAllocator::printFreeLists() {
    std::cout << "========================== Buddy Free Lists ============================" << std::endl;
    for (unsigned int i = max_order_; i >= min_order_; i--) {
        std::cout << i << " ";
        Block* p = free_list_by_order_[i];
        while (p != nullptr) {
            std::cout << p << " ";
            p = p->next;
        }
        std::cout << std::endl;
    }
    std::cout << "========================================================================" << std::endl;
}

