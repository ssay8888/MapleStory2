#pragma once

class SendBufferChunk;

class SendBuffer
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize);
	~SendBuffer();

	auto Buffer() const -> BYTE* ;
	auto AllocSize() const -> uint32 ;
	auto WriteSize() const -> uint32 ;
	auto Close(uint32 writeSize) -> void;

private:
	BYTE*				_buffer;
	uint32				_alloc_size = 0;
	uint32				_write_size = 0;
	SendBufferChunkRef	_owner;
};

class SendBufferChunk : public std::enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000
	};

public:
	SendBufferChunk() = default;
	~SendBufferChunk() = default;

	auto Reset() -> void;
	auto Open(uint32 allocSize) -> SendBufferRef;
	auto Close(uint32 writeSize) -> void;

	auto IsOpen() const -> bool { return _open; }
	auto Buffer() -> BYTE* { return &_buffer[_used_size]; }
	auto FreeSize() const -> uint32 { return static_cast<uint32>(_buffer.size()) - _used_size; }

private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE>		_buffer = {};
	bool									_open = false;
	uint32									_used_size = 0;
};

/*---------------------
	SendBufferManager
----------------------*/

class SendBufferManager
{
private:
	SendBufferManager() = default;
	~SendBufferManager() = default;
public:
	static auto& GetInstance()
	{
		static SendBufferManager instance;
		return instance;
	}

	SendBufferRef		Open(uint32 size);

private:
	auto Pop() -> SendBufferChunkRef;
	auto Push(SendBufferChunkRef buffer) -> void;

	static auto PushGlobal(SendBufferChunk* buffer) -> void;

private:
	USE_LOCK;
	Vector<SendBufferChunkRef> _sendBufferChunks;
};
