#include "pch.h"
#include "buffer_writer.h"

BufferWriter::BufferWriter()
{

}

BufferWriter::BufferWriter(BYTE* buffer, uint32_t size, uint32_t pos) :
	_buffer(buffer),
	_size(size),
	_pos(pos)
{

}

auto BufferWriter::Buffer() const -> BYTE*
{
	return _buffer;
}

auto BufferWriter::Size() const -> uint32_t
{
	return _size;
}

auto BufferWriter::WriteSize() const -> uint32_t
{
	return _pos;
}

auto BufferWriter::FreeSize() const -> uint32_t
{
	return _size - _pos;
}

bool BufferWriter::Write(const void* src, const uint32_t len)
{
	if (FreeSize() < len)
		return false;

	::memcpy(&_buffer[_pos], src, len);
	_pos += len;
	return true;
}
