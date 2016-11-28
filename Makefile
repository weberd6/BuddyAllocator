CXX=g++
CXXFLAGS=-std=c++11

BuddyAllocatorTest: BuddyAllocatorTest.o BuddyAllocator.o
	$(CXX) BuddyAllocator.o BuddyAllocatorTest.o -o BuddyAllocatorTest 

BuddyAllocatorTest.o: BuddyAllocatorTest.cpp
	$(CXX) -c $(CXXFLAGS) BuddyAllocatorTest.cpp

BuddyAllocator.o: BuddyAllocator.cpp BuddyAllocator.hpp
	$(CXX) -c $(CXXFLAGS) BuddyAllocator.cpp

.PHONY: clean

clean:
	rm -rf *.o BuddyAllocatorTest
