#pragma once
#include "src/utility/vi_buffer/vi_buffer.h"

class ViBufferRect : public ViBuffer
{
public:
	explicit ViBufferRect(const ComPtr<IDirect3DDevice9>& graphicDevice);
	virtual ~ViBufferRect() = default;
	
public:
	auto NativeConstruct(void* arg)->HRESULT override;
	auto NativeConstructPrototype() -> HRESULT override;
	auto Clone(void* pArg) -> std::shared_ptr<Component> override;
};

