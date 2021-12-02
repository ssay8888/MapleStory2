#pragma once

enum
{
	kListAlignment = 16
};

DECLSPEC_ALIGN(kListAlignment)
struct MemoryHeader : public SLIST_ENTRY
{
	MemoryHeader(const int32_t size) : alloc_size(size) { }

	static void* AttachHeader(MemoryHeader* header, int32_t size)
	{
		new(header)MemoryHeader(size); 
		return ++header;
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32_t alloc_size;
	// TODO : 필요한 추가 정보
};

DECLSPEC_ALIGN(kListAlignment)
class MemoryPool
{
public:
	MemoryPool(int32_t allocSize);
	~MemoryPool();

	auto Push(MemoryHeader* ptr) -> void;
	auto Pop() -> MemoryHeader*;

private:
	SLIST_HEADER		_header{};
	int32_t				_alloc_size = 0;
	std::atomic<int32_t>	_use_count = 0;
	std::atomic<int32_t>	_reserve_count = 0;
};

