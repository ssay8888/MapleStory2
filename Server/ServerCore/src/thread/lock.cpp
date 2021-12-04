#include "pch.h"
#include "lock.h"
#include "core_tls.h"
#include "dead_lock_profiler.h"

void Lock::WriteLock(const char* name)
{
#if _DEBUG
	//DeadLockProfiler::GetInstance().PushLock(name);
#endif
	
	const uint32_t lockThreadId = (_lock_flag.load() & kWriteThreadMask) >> 16;
	if (LThreadId == lockThreadId)
	{
		_write_count++;
		return;
	}
	
	const int64_t beginTick = GetTickCount64();
	const uint32_t desired = ((LThreadId << 16) & kWriteThreadMask);
	while (true)
	{
		for (uint32_t spinCount = 0; spinCount < kMaxSpinCount; spinCount++)
		{
			uint32_t expected = kEmptyFlag;
			if (_lock_flag.compare_exchange_strong(OUT expected, desired))
			{
				_write_count++;
				return;
			}
		}

		if (GetTickCount64() - beginTick >= kAcquireTimeoutTick)
			CRASH("LOCK_TIMEOUT");
		_mm_pause();
	}
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	//DeadLockProfiler::GetInstance().PopLock(name);
#endif
	
	if ((_lock_flag.load() & kReadCountMask) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32_t lockCount = --_write_count;
	if (lockCount == 0)
		_lock_flag.store(kEmptyFlag);
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	//DeadLockProfiler::GetInstance().PushLock(name);
#endif
	
	const uint32_t lockThreadId = (_lock_flag.load() & kWriteThreadMask) >> 16;
	if (LThreadId == lockThreadId)
	{
		_lock_flag.fetch_add(1);
		return;
	}
	
	const int64_t beginTick = GetTickCount64();
	while (true)
	{
		for (uint32_t spinCount = 0; spinCount < kMaxSpinCount; spinCount++)
		{
			uint32_t expected = (_lock_flag.load() & kReadCountMask);
			if (_lock_flag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}

		if (GetTickCount64() - beginTick >= kAcquireTimeoutTick)
			CRASH("LOCK_TIMEOUT");

		_mm_pause();
	}
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	//DeadLockProfiler::GetInstance().PopLock(name);
#endif

	if ((_lock_flag.fetch_sub(1) & kReadCountMask) == 0)
		CRASH("MULTIPLE_UNLOCK");
}

ReadLockGuard::ReadLockGuard(Lock& lock, const char* name) :
	_lock(lock),
	_name(name)
{
	_lock.ReadLock(name);
}

ReadLockGuard::~ReadLockGuard()
{
	_lock.ReadUnlock(_name);
}

WriteLockGuard::WriteLockGuard(Lock& lock, const char* name) :
	_lock(lock),
	_name(name)
{
	_lock.WriteLock(name);
}

WriteLockGuard::~WriteLockGuard()
{
	_lock.WriteUnlock(_name);
}
