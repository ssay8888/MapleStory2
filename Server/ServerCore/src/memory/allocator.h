#pragma once

/*-------------------
	BaseAllocator
-------------------*/

class BaseAllocator
{
public:
	static void*	Alloc(int32_t size);
	static void		Release(void* ptr);
};

/*-------------------
	StompAllocator
-------------------*/

class StompAllocator
{
	enum { kPageSize = 0x1000 };

public:
	static auto	Alloc(int32_t size) -> void*;
	static auto	Release(void* ptr) -> void;
};

/*-------------------
	PoolAllocator
-------------------*/

class PoolAllocator
{
public:
	static auto	Alloc(int32_t size) -> void*;
	static auto	Release(void* ptr) -> void;
};

/*-------------------
	STL Allocator
-------------------*/

template<typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator() = default;

	template<typename Other>
	StlAllocator(const StlAllocator<Other>&) { }

	T* allocate(size_t count)
	{
		const int32_t size = static_cast<int32_t>(count * sizeof(T));
		return static_cast<T*>(PoolAllocator::Alloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		PoolAllocator::Release(ptr);
	}

	template<typename U>
	bool operator==(const StlAllocator<U>&) { return true; }

	template<typename U>
	bool operator!=(const StlAllocator<U>&) { return false; }
};