#pragma once

class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int32_t bufferSize);
	~RecvBuffer();

	auto Clean() -> void;
	auto OnRead(int32_t numOfBytes)  -> bool;
	auto OnWrite(int32_t numOfBytes) -> bool;
		 
	auto ReadPos() -> BYTE* ;
	auto WritePos() -> BYTE* ;
	auto DataSize() const -> int32_t ;
	auto FreeSize() const -> int32_t ;

private:
	int32_t			_capacity = 0;
	int32_t			_buffer_size = 0;
	int32_t			_read_pos = 0;
	int32_t			_write_pos = 0;
	Vector<BYTE>	_buffer;
};

