#pragma once

enum
{
	kListAlignment = 16
};

DECLSPEC_ALIGN(kListAlignment)
struct MemoryHeader : public SLIST_ENTRY
{
	MemoryHeader(const int32 size) : alloc_size(size) { }

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size); 
		return ++header;
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 alloc_size;
	// TODO : 필요한 추가 정보
};

DECLSPEC_ALIGN(kListAlignment)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	auto Push(MemoryHeader* ptr) -> void;
	auto Pop() -> MemoryHeader*;

private:
	SLIST_HEADER		_header{};
	int32				_alloc_size = 0;
	std::atomic<int32>	_use_count = 0;
	std::atomic<int32>	_reserve_count = 0;
};

