#include "pch.h"
#include "hierarchy_loader.h"

HierarchyLoader::HierarchyLoader(const ComPtr<IDirect3DDevice9>& device)
{
}

HRESULT HierarchyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME* ppNewFrame)
{
	D3DxFrameDerived* pFrame = new D3DxFrameDerived;



	*ppNewFrame = pFrame;

	return S_OK;
}

HRESULT HierarchyLoader::CreateMeshContainer(LPCSTR Name, const D3DXMESHDATA* pMeshData, const D3DXMATERIAL* pMaterials,
	const D3DXEFFECTINSTANCE* pEffectInstances, DWORD NumMaterials, const DWORD* pAdjacency, LPD3DXSKININFO pSkinInfo,
	LPD3DXMESHCONTAINER* ppNewMeshContainer)
{
	return S_OK;
}

HRESULT HierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	return S_OK;
}

HRESULT HierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	return S_OK;
}

auto HierarchyLoader::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<HierarchyLoader>
{
	return std::make_shared<HierarchyLoader>(device);
}
