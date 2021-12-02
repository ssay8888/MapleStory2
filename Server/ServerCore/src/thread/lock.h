#pragma once
#include "types.h"

/*--------------------------------------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : WriteFlag (Exclusive Lock Owner ThreadId)
R : ReadFlag (Shared Lock Count)
---------------------------------------------*/

class Lock
{
    enum : uint32_t
    {
        kAcquireTimeoutTick = 10000,
        kMaxSpinCount = 5000,
        kWriteThreadMask = 0xFFFF'0000,
        kReadCountMask = 0x0000'FFFF,
        kEmptyFlag = 0x0000'0000
    };

public:
    auto WriteLock(const char* name) -> void;
    auto WriteUnlock(const char* name) -> void;
    auto ReadLock(const char* name) -> void;
    auto ReadUnlock(const char* name) -> void;

private:
    Atomic<uint32_t> _lock_flag = kEmptyFlag;
    uint16_t _write_count = 0;
};

class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock, const char* name);
	~ReadLockGuard();

private:
	Lock& _lock;
    const char* _name;
};


class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name);
	~WriteLockGuard();

private:
	Lock& _lock;
    const char* _name;
};
