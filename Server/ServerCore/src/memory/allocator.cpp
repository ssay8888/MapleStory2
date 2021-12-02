#include "pch.h"
#include "allocator.h"
#include "memory.h"

/*-------------------
	BaseAllocator
-------------------*/

auto BaseAllocator::Alloc(int32_t size) -> void*
{
	return ::malloc(size);
}

auto BaseAllocator::Release(void* ptr) -> void
{
	::free(ptr);
}

/*-------------------
	StompAllocator
-------------------*/

auto StompAllocator::Alloc(int32_t size) -> void*
{
	const int64_t pageCount = (size + kPageSize - 1) / kPageSize;
	const int64_t dataOffset = pageCount * kPageSize - size;
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * kPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8_t*>(baseAddress) + dataOffset);
}

auto StompAllocator::Release(void* ptr) -> void
{
	const int64_t address = reinterpret_cast<int64_t>(ptr);
	const int64_t baseAddress = address - (address % kPageSize);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}

/*-------------------
	PoolAllocator
-------------------*/

auto PoolAllocator::Alloc(int32_t size) -> void*
{
	return Memory::GetInstance().Allocate(size);
}

auto PoolAllocator::Release(void* ptr) -> void
{
	Memory::GetInstance().Release(ptr);
}