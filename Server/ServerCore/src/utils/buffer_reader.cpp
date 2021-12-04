#include "pch.h"
#include "buffer_reader.h"

BufferReader::BufferReader(BYTE* buffer, const uint32_t size, const uint32_t pos) :
	_buffer(buffer),
	_size(size),
	_pos(pos)
{

}

auto BufferReader::Buffer() const -> BYTE*
{
	return _buffer;
}

auto BufferReader::Size() const -> uint32_t
{
	return _size;
}

auto BufferReader::ReadSize() const -> uint32_t
{
	return _pos;
}

auto BufferReader::FreeSize() const -> uint32_t
{
	return _size - _pos;
}

bool BufferReader::Peek(void* dest, const uint32_t len) const
{
	if (FreeSize() < len)
		return false;

	memcpy(dest, &_buffer[_pos], len);
	return true;
}

bool BufferReader::Read(void* dest, const uint32_t len)
{
	if (Peek(dest, len) == false)
		return false;

	_pos += len;
	return true;
}
