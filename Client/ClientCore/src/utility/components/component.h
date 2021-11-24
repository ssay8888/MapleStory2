#pragma once
class Component
{
protected:
	explicit Component(const ComPtr<IDirect3DDevice9>& graphicDevice);
	explicit Component(const Component& rhs);
	virtual ~Component() = default;
public:
	virtual auto NativeConstructPrototype()->HRESULT;
	virtual auto NativeConstruct(void* pArg)->HRESULT;

public:
	virtual auto Clone(void* pArg = nullptr)->std::shared_ptr<Component> = 0;


protected:
	ComPtr<IDirect3DDevice9>		_graphic_device = nullptr;
	bool							_is_clone = false;
};

