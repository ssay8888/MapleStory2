#pragma once

class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();

	auto Clean() -> void;
	auto OnRead(int32 numOfBytes)  -> bool;
	auto OnWrite(int32 numOfBytes) -> bool;
		 
	auto ReadPos() -> BYTE* ;
	auto WritePos() -> BYTE* ;
	auto DataSize() const -> int32 ;
	auto FreeSize() const -> int32 ;

private:
	int32			_capacity = 0;
	int32			_buffer_size = 0;
	int32			_read_pos = 0;
	int32			_write_pos = 0;
	Vector<BYTE>	_buffer;
};

