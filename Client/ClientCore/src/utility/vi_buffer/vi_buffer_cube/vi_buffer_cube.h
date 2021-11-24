#pragma once
#include "src/utility/vi_buffer/vi_buffer.h"

class ViBufferCube final : public ViBuffer
{
public:
	explicit ViBufferCube(const ComPtr<IDirect3DDevice9>& graphicDevice);
	virtual ~ViBufferCube() = default;
public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;

public:
	static std::shared_ptr<ViBufferCube> Create(const ComPtr<IDirect3DDevice9>& graphicDevice);
	std::shared_ptr<Component> Clone(void* arg = nullptr) override;
};

