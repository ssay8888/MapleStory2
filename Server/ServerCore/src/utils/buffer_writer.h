#pragma once

class BufferWriter
{
public:
	BufferWriter();
	BufferWriter(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferWriter() = default;

	auto Buffer() const -> BYTE*;
	auto Size() const -> uint32;
	auto WriteSize() const -> uint32;
	auto FreeSize() const -> uint32;

	template<typename T>
	auto Write(T* src) -> bool { return Write(src, sizeof(T)); }
	auto Write(const void* src, const uint32 len) -> bool;

	template<typename T>
	auto Reserve(uint16 count = 1) -> T*;

	template<typename T>
	auto operator<<(T&& src) -> BufferWriter&;

private:
	BYTE*			_buffer = nullptr;
	uint32			_size = 0;
	uint32			_pos = 0;
};

template<typename T>
T* BufferWriter::Reserve(const uint16 count)
{
	if (FreeSize() < (sizeof(T) * count))
		return nullptr;

	T* ret = reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += (sizeof(T) * count);
	return ret;
}

template<typename T>
BufferWriter& BufferWriter::operator<<(T&& src)
{
	using DataType = std::remove_reference_t<T>;
	*reinterpret_cast<DataType*>(&_buffer[_pos]) = std::forward<DataType>(src);
	_pos += sizeof(T);
	return *this;
}