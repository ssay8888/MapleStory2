#pragma once
class HierarchyLoader final : public ID3DXAllocateHierarchy
{
public:
	explicit HierarchyLoader(const ComPtr<IDirect3DDevice9>& device);
	virtual ~HierarchyLoader() = default;

	HRESULT CreateFrame(LPCSTR Name, LPD3DXFRAME* ppNewFrame) override;
	HRESULT CreateMeshContainer(LPCSTR Name, const D3DXMESHDATA* pMeshData, const D3DXMATERIAL* pMaterials,
		const D3DXEFFECTINSTANCE* pEffectInstances, DWORD NumMaterials, const DWORD* pAdjacency,
		LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER* ppNewMeshContainer) override;
	HRESULT DestroyFrame(LPD3DXFRAME pFrameToFree) override;
	HRESULT DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree) override;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<HierarchyLoader>;
};

