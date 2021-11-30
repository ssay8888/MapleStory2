#include "pch.h"
#include "mesh.h"

Mesh::Mesh(const ComPtr<IDirect3DDevice9>& device):
	Component(device)
{
}

HRESULT Mesh::NativeConstructPrototype()
{
	return Component::NativeConstructPrototype();
}

HRESULT Mesh::NativeConstruct(void* arg)
{
	return Component::NativeConstruct(arg);
}