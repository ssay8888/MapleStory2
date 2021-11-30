#include "pch.h"
#include "mesh_static.h"

MeshStatic::MeshStatic(const ComPtr<IDirect3DDevice9>& device):
	Mesh(device)
{
}

auto MeshStatic::NativeConstructPrototype(const std::wstring& filePath, const std::wstring& fileName) -> HRESULT
{
	wchar_t		szFullPath[MAX_PATH] = TEXT("");

	lstrcpy(szFullPath, filePath.c_str());
	lstrcat(szFullPath, fileName.c_str());
	LPD3DXMESH	mesh = nullptr;

	if (FAILED(D3DXLoadMeshFromX(szFullPath, D3DXMESH_MANAGED, _graphic_device.Get(), &_adjacency, &_materials, nullptr, &_num_materials, &mesh)))
		return E_FAIL;

	_mesh.Attach(mesh);

	return S_OK;
}

HRESULT MeshStatic::NativeConstruct(void* arg)
{
	return Mesh::NativeConstruct(arg);
}

std::shared_ptr<Component> MeshStatic::Clone(void* arg)
{
	auto pInstance = std::make_shared<MeshStatic>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone MeshStatic");
		return nullptr;
	}
	return pInstance;
}

auto MeshStatic::Render() -> HRESULT
{
	if (nullptr == _mesh)
		return E_FAIL;

	for (uint32_t i = 0; i < _num_materials; ++i)
		_mesh->DrawSubset(i);
	return S_OK;
}

auto MeshStatic::Create(const ComPtr<IDirect3DDevice9>& device, const std::wstring& filePath,
	const std::wstring& fileName) -> std::shared_ptr<MeshStatic>
{
	auto pInstance = std::make_shared<MeshStatic>(device);

	if (FAILED(pInstance->NativeConstructPrototype(filePath, fileName)))
	{
		MSGBOX("Failed to Creating MeshStatic");
		return nullptr;
	}
	return pInstance;
}
