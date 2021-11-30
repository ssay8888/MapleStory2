#pragma once
#include "src/utility/components/component.h"

class Mesh : public Component
{
public:
	explicit Mesh(const ComPtr<IDirect3DDevice9>& device);
	virtual ~Mesh() = default;
public:
	virtual auto NativeConstructPrototype()->HRESULT;
	virtual auto NativeConstruct(void* arg)->HRESULT;
public:
	virtual std::shared_ptr<Component> Clone(void* arg = nullptr) = 0;
};

