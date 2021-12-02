#pragma once

class BufferReader
{
public:
	BufferReader() = default;
	BufferReader(BYTE* buffer, uint32_t size, uint32_t pos = 0);

	~BufferReader() = default;

public:
	auto Buffer() const -> BYTE* ;
	auto Size() const -> uint32_t ;
	auto ReadSize() const -> uint32_t ;
	auto FreeSize() const -> uint32_t ;

	template<typename T>
	auto Peek(T* dest) -> bool { return Peek(dest, sizeof(T)); }
	auto Peek(void* dest, uint32_t len) const -> bool;

	template<typename T>
	auto Read(T* dest) -> bool { return Read(dest, sizeof(T)); }
	auto Read(void* dest, uint32_t len) -> bool;

	template<typename T>
	auto operator>>(OUT T& dest) -> BufferReader&;

private:
	BYTE*			_buffer = nullptr;
	uint32_t			_size = 0;
	uint32_t			_pos = 0;
};

template<typename T>
inline auto BufferReader::operator>>(OUT T& dest) -> BufferReader&
{
	dest = *reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return *this;
}