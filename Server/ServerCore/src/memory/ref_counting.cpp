#include "pch.h"
#include "ref_counting.h"

RefCountable::RefCountable() :
	_ref_count(1)
{
	
}

auto RefCountable::GetRefCount() const -> int32_t
{
	return _ref_count;
}

auto RefCountable::AddRef() -> int32_t
{
	return ++_ref_count;
}

auto RefCountable::ReleaseRef() -> int32_t
{
	const int32_t refCount = --_ref_count;
	if (refCount == 0)
	{
		delete this;
	}
	return refCount;
}
