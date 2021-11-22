#include "pch.h"
#include "core_tls.h"

thread_local uint32				LThreadId = 0;
thread_local uint64				LEndTickCount = 0;
thread_local std::stack<int32>	LLockStack;
thread_local SendBufferChunkRef	LSendBufferChunk;
thread_local JobQueue*			LCurrentJobQueue = nullptr;
thread_local std::queue<std::pair<PacketSessionRef, BYTE*>>	LRecvBuffers;