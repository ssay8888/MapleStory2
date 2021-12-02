#include "pch.h"
#include "memory.h"
#include "memory_pool.h"

Memory::Memory()
{
	int32_t size = 0;
	int32_t tableIndex = 0;

	for (size = 32; size <= 1024; size += 32)
	{
		auto pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_pool_table[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size <= 2048; size += 128)
	{
		auto pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_pool_table[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size <= 4096; size += 256)
	{
		auto pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_pool_table[tableIndex] = pool;
			tableIndex++;
		}
	}
}

Memory::~Memory()
{
	for (const MemoryPool* pool : _pools)
		delete pool;

	_pools.clear();
}

auto Memory::Allocate(const int32_t size) -> void*
{
	MemoryHeader* header = nullptr;
	const int32_t allocSize = size + sizeof(MemoryHeader);

#ifdef _STOMP
	header = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(allocSize));
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// �޸� Ǯ�� �ִ� ũ�⸦ ����� �Ϲ� �Ҵ�
		header = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		// �޸� Ǯ���� �����´�
		header = _poolTable[allocSize]->Pop();
	}
#endif	

	return MemoryHeader::AttachHeader(header, allocSize);
}

auto Memory::Release(void* ptr) -> void
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

	const int32_t allocSize = header->alloc_size;
	ASSERT_CRASH(allocSize > 0);

#ifdef _STOMP
	StompAllocator::Release(header);
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// �޸� Ǯ�� �ִ� ũ�⸦ ����� �Ϲ� ����
		::_aligned_free(header);
	}
	else
	{
		// �޸� Ǯ�� �ݳ��Ѵ�
		_poolTable[allocSize]->Push(header);
	}
#endif	
}
