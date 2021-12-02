#include "pch.h"
#include "mesh_dynamic.h"

#include "loader/hierarchy_loader.h"

MeshDynamic::MeshDynamic(const ComPtr<IDirect3DDevice9>& device):
	Mesh(device)
{
}

auto MeshDynamic::NativeConstructPrototype(const std::wstring& filePath, const std::wstring& fileName) -> HRESULT
{
	wchar_t		szFullPath[MAX_PATH] = TEXT("");

	lstrcpy(szFullPath, filePath.c_str());
	lstrcat(szFullPath, fileName.c_str());

	const std::shared_ptr<HierarchyLoader> hierarchyLoader = HierarchyLoader::Create(_graphic_device);

	if (FAILED(D3DXLoadMeshHierarchyFromX(szFullPath, D3DXMESH_MANAGED, _graphic_device.Get(), hierarchyLoader.get(), nullptr, &_root_frame, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT MeshDynamic::NativeConstruct(void* arg)
{
	return S_OK;
}

auto MeshDynamic::Create(const ComPtr<IDirect3DDevice9>& device, const std::wstring& filePath,
	const std::wstring& fileName) -> std::shared_ptr<MeshDynamic>
{
	auto pInstance = std::make_shared<MeshDynamic>(device);

	if (FAILED(pInstance->NativeConstructPrototype(filePath, fileName)))
	{
		MSGBOX("Failed to Creating CMesh_Dynamic");
		return nullptr;
	}
	return pInstance;
}

auto MeshDynamic::Clone(void* arg) -> std::shared_ptr<Component>
{
	auto pInstance = std::make_shared<MeshDynamic>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone CMesh_Dynamic");
		return nullptr;
	}
	return pInstance;
}
