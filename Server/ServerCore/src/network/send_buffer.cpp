#include "pch.h"
#include "send_buffer.h"

SendBuffer::SendBuffer(const SendBufferChunkRef owner, BYTE* buffer, uint32_t allocSize) :
	_owner(owner),
	_buffer(buffer),
	_alloc_size(allocSize)
{
}

SendBuffer::~SendBuffer()
{
}

auto SendBuffer::Buffer() const -> BYTE*
{
	return _buffer;
}

auto SendBuffer::AllocSize() const -> uint32_t
{
	return _alloc_size;
}

auto SendBuffer::WriteSize() const -> uint32_t
{
	return _write_size;
}

void SendBuffer::Close(uint32_t writeSize)
{
	ASSERT_CRASH(_alloc_size >= writeSize);
	_write_size = writeSize;
	_owner->Close(writeSize);
}

void SendBufferChunk::Reset()
{
	_open = false;
	_used_size = 0;
}

SendBufferRef SendBufferChunk::Open(uint32_t allocSize) 
{
	ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
	ASSERT_CRASH(_open == false);

	if (allocSize > FreeSize())
		return nullptr;

	_open = true;
	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), allocSize);
}

void SendBufferChunk::Close(uint32_t writeSize)
{
	ASSERT_CRASH(_open == true);
	_open = false;
	_used_size += writeSize;
}

/*---------------------
	SendBufferManager
----------------------*/

SendBufferRef SendBufferManager::Open(uint32_t size)
{
	if (LSendBufferChunk == nullptr)
	{
		LSendBufferChunk = Pop(); // WRITE_LOCK
		LSendBufferChunk->Reset();
	}		

	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

	// 다 썼으면 버리고 새거로 교체
	if (LSendBufferChunk->FreeSize() < size)
	{
		LSendBufferChunk = Pop(); // WRITE_LOCK
		LSendBufferChunk->Reset();
	}

	return LSendBufferChunk->Open(size);
}

auto SendBufferManager::Pop() -> SendBufferChunkRef
{
	{
		WRITE_LOCK;
		if (_sendBufferChunks.empty() == false)
		{
			SendBufferChunkRef sendBufferChunk = _sendBufferChunks.back();
			_sendBufferChunks.pop_back();
			return sendBufferChunk;
		}
	}

	return SendBufferChunkRef(xnew<SendBufferChunk>(), PushGlobal);
}

auto SendBufferManager::Push(SendBufferChunkRef buffer) -> void
{
	WRITE_LOCK;
	_sendBufferChunks.push_back(buffer);
}

auto SendBufferManager::PushGlobal(SendBufferChunk* buffer) -> void
{
	std::cout << "PushGlobal SENDBUFFERCHUNK" << std::endl;

	GetInstance().Push(SendBufferChunkRef(buffer, PushGlobal));
}