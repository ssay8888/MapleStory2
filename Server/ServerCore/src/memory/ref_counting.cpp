#include "pch.h"
#include "ref_counting.h"

RefCountable::RefCountable() :
	_ref_count(1)
{
	
}

auto RefCountable::GetRefCount() const -> int32
{
	return _ref_count;
}

auto RefCountable::AddRef() -> int32
{
	return ++_ref_count;
}

auto RefCountable::ReleaseRef() -> int32
{
	const int32 refCount = --_ref_count;
	if (refCount == 0)
	{
		delete this;
	}
	return refCount;
}
