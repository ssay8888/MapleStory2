#pragma once
#include "types.h"
#include "memory_pool.h"

template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
#ifdef _STOMP
		const auto ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(_s_alloc_size));
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(ptr, _s_alloc_size));
#else
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
#endif		
		new(memory)Type(std::forward<Args>(args)...); // placement new
		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type();
#ifdef _STOMP
		StompAllocator::Release(MemoryHeader::DetachHeader(obj));
#else
		s_pool.Push(MemoryHeader::DetachHeader(obj));
#endif
	}

	template<typename... Args>
	static std::shared_ptr<Type> MakeShared(Args&&... args)
	{
		std::shared_ptr<Type> ptr = { Pop(std::forward<Args>(args)...), Push };
		return ptr;
	}

private:
	static int32		_s_alloc_size;
	static MemoryPool	_s_pool;
};

template<typename Type>
int32 ObjectPool<Type>::_s_alloc_size = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::_s_pool { _s_alloc_size };