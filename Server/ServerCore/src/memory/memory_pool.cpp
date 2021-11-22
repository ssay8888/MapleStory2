#include "pch.h"
#include "memory_pool.h"

MemoryPool::MemoryPool(const int32 allocSize) :
	_alloc_size(allocSize)
{
	::InitializeSListHead(&_header);
}

MemoryPool::~MemoryPool()
{
	while (auto memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)))
		::_aligned_free(memory);
}

auto MemoryPool::Push(MemoryHeader* ptr) -> void
{
	ptr->alloc_size = 0;

	::InterlockedPushEntrySList(&_header, ptr);

	_use_count.fetch_sub(1);
	_reserve_count.fetch_add(1);
}

auto MemoryPool::Pop() -> MemoryHeader*
{
	auto memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header));

	if (memory == nullptr)
	{
		memory = static_cast<MemoryHeader*>(::_aligned_malloc(_alloc_size, kListAlignment));
	}
	else
	{
		ASSERT_CRASH(memory->alloc_size == 0);
		_reserve_count.fetch_sub(1);
	}

	_use_count.fetch_add(1);

	return memory;
}
