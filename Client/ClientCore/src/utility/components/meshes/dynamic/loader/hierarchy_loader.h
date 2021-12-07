#pragma once
class HierarchyLoader final : public ID3DXAllocateHierarchy
{
public:
	explicit HierarchyLoader(const ComPtr<IDirect3DDevice9>& device);
	virtual ~HierarchyLoader() = default;

public:
	auto NativeConstruct(const std::wstring& meshFilePath)->HRESULT;


	auto CreateFrame(LPCSTR Name, LPD3DXFRAME* ppNewFrame)->HRESULT override;
	auto CreateMeshContainer(LPCSTR Name, const D3DXMESHDATA* pMeshData, const D3DXMATERIAL* pMaterials,
		const D3DXEFFECTINSTANCE* pEffectInstances, DWORD NumMaterials, const DWORD* pAdjacency,
		LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER* ppNewMeshContainer)->HRESULT override;
	auto DestroyFrame(LPD3DXFRAME pFrameToFree)-> HRESULT override;
	auto DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)->HRESULT override;
	
public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device, const std::wstring& pMeshFilePath)->std::shared_ptr<HierarchyLoader>;

private:
	auto AllocateName(const char* pSourName, char** ppDestName)->void;

private:
	std::wstring _mesh_file_path;
};