#pragma once
#include "allocator.h"

class MemoryPool;

class Memory
{
	enum
	{
		kPoolCount = (1024 / 32) + (1024 / 128) + (2048 / 256),
		kMaxAllocSize = 4096
	};

private:
	Memory();
	~Memory();
public:

	static auto& GetInstance()
	{
		static Memory instance;
		return instance;
	}
	auto Allocate(int32 size) -> void*;
	auto Release(void* ptr) -> void;

private:
	std::vector<MemoryPool*> _pools;
	MemoryPool* _pool_table[kMaxAllocSize + 1]{nullptr, };
};


template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	new(memory)Type(std::forward<Args>(args)...); // placement new
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();
	PoolAllocator::Release(obj);
}

template<typename Type, typename... Args>
std::shared_ptr<Type> MakeShared(Args&&... args)
{
	return std::shared_ptr<Type>{ xnew<Type>(std::forward<Args>(args)...), xdelete<Type> };
}