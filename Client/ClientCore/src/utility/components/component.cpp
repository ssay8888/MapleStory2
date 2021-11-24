#include "pch.h"
#include "component.h"

Component::Component(const ComPtr<IDirect3DDevice9>& graphicDevice) :
	_graphic_device(graphicDevice),
	_is_clone(false)
{
	Component::NativeConstructPrototype();
}

Component::Component(const Component& rhs) :
	_graphic_device(rhs._graphic_device),
	_is_clone(true)
{
}

auto Component::NativeConstructPrototype() -> HRESULT
{
	return S_OK;
}

auto Component::NativeConstruct(void* pArg) -> HRESULT
{
	return S_OK;
}