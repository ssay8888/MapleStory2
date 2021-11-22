#include "pch.h"
#include "recv_buffer.h"

RecvBuffer::RecvBuffer(int32 bufferSize) : _buffer_size(bufferSize)
{
	_capacity = bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

auto RecvBuffer::Clean() -> void
{
	int32 dataSize = DataSize();
	std::cout << dataSize << "dataSize" << std::endl;
	if (dataSize == 0)
	{
		_read_pos = _write_pos = 0;
	}
	else
	{
		if (FreeSize() < _buffer_size)
		{
			::memcpy(&_buffer[0], &_buffer[_read_pos], dataSize);
			_read_pos = 0;
			_write_pos = dataSize;
		}
	}
}

auto RecvBuffer::OnRead(int32 numOfBytes) -> bool
{
	if (numOfBytes > DataSize())
		return false;

	_read_pos += numOfBytes;
	return true;
}

auto RecvBuffer::OnWrite(int32 numOfBytes) -> bool
{
	if (numOfBytes > FreeSize())
		return false;

	_write_pos += numOfBytes;
	return true;
}

auto RecvBuffer::ReadPos() -> BYTE*
{
	return &_buffer[_read_pos];
}

auto RecvBuffer::WritePos() -> BYTE*
{
	return &_buffer[_write_pos];
}

auto RecvBuffer::DataSize() const -> int32
{
	return _write_pos - _read_pos;
}

auto RecvBuffer::FreeSize() const -> int32
{
	return _capacity - _write_pos;
}
