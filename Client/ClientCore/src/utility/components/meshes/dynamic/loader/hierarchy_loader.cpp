#include "pch.h"
#include "hierarchy_loader.h"

#include "src/system/graphic/graphic_device.h"

HierarchyLoader::HierarchyLoader(const ComPtr<IDirect3DDevice9>& device)
{
}

HRESULT HierarchyLoader::NativeConstruct(const std::wstring& meshFilePath)
{
	_mesh_file_path = meshFilePath;
	return S_OK;
}

auto HierarchyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME* ppNewFrame)->HRESULT
{
	const auto pFrame = new D3DxFrameDerived;

	ZeroMemory(pFrame, sizeof(D3DxFrameDerived));

	if (nullptr != Name)
	{
		AllocateName(Name, &pFrame->Name);
	}

	D3DXMatrixIdentity(&pFrame->TransformationMatrix);
	D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);

	*ppNewFrame = pFrame;

	return S_OK;
}

auto HierarchyLoader::CreateMeshContainer(LPCSTR Name, const D3DXMESHDATA* pMeshData, const D3DXMATERIAL* pMaterials,
	const D3DXEFFECTINSTANCE* pEffectInstances, DWORD NumMaterials, const DWORD* pAdjacency, LPD3DXSKININFO pSkinInfo,
	LPD3DXMESHCONTAINER* ppNewMeshContainer)->HRESULT
{
	auto device = GraphicDevice::GetInstance().GetDevice().Get();
	D3DXMeshContainerDerived* pMeshContainer = new D3DXMeshContainerDerived;
	ZeroMemory(pMeshContainer, sizeof(D3DXMeshContainerDerived));

	if (nullptr != Name)
		AllocateName(Name, &pMeshContainer->Name);

	D3DVERTEXELEMENT9			Element[MAX_FVF_DECL_SIZE];

	pMeshData->pMesh->GetDeclaration(Element);

	/* ������ �޽ø� �����Ѵ�. */
	if (FAILED(pMeshData->pMesh->CloneMesh(pMeshData->pMesh->GetOptions(), Element, device, &pMeshContainer->MeshData.pMesh)))
		return E_FAIL;
	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	pMeshContainer->NumMaterials = NumMaterials;

	pMeshContainer->ppMaterialTextures = new MeshMaterialTexture * [NumMaterials];

	for (size_t i = 0; i < NumMaterials; ++i)
		pMeshContainer->ppMaterialTextures[i] = new MeshMaterialTexture;




	for (size_t i = 0; i < pMeshContainer->NumMaterials; ++i)
	{
		wchar_t		szFullPath[MAX_PATH] = TEXT("");

		lstrcpy(szFullPath, _mesh_file_path.c_str());

		wchar_t		szTextureFileName[MAX_PATH] = TEXT("");

		MultiByteToWideChar(CP_ACP, 0, pMaterials[i].pTextureFilename, static_cast<int32_t>(strlen(pMaterials[i].pTextureFilename))
			, szTextureFileName, MAX_PATH);

		lstrcat(szFullPath, szTextureFileName);

		int32_t		iLength = 0;

		iLength = lstrlen(szFullPath);
		int32_t	iIndex = 0;

		for (int32_t i = iLength; i >= 0; --i)
		{
			if (szFullPath[i] == L'.')
			{
				iIndex = i - 1;
				break;
			}
		}


		/* For.Diffuse map */
		szFullPath[iIndex] = L'D';
		D3DXCreateTextureFromFile(device, szFullPath, &pMeshContainer->ppMaterialTextures[i]->diffuse_map);


		/* For.Normal map */
		szFullPath[iIndex] = L'N';
		D3DXCreateTextureFromFile(device, szFullPath, &pMeshContainer->ppMaterialTextures[i]->normal_map);


		/* For.Specular map */
		szFullPath[iIndex] = L'S';
		D3DXCreateTextureFromFile(device, szFullPath, &pMeshContainer->ppMaterialTextures[i]->specular_map);
	}
	const uint64_t numFaces = pMeshContainer->MeshData.pMesh->GetNumFaces();
	uint64_t size = static_cast<uint64_t>(numFaces * 3);
	pMeshContainer->pAdjacency = new DWORD[size];
	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * pMeshContainer->MeshData.pMesh->GetNumFaces() * 3);

	/* ���� �޽�. ��ġ �븻 ������ ź��Ʈ ���̳븻 �����ε���, ���ο���Ʈ */

	pMeshContainer->pSkinInfo = pSkinInfo;


	pMeshContainer->pSkinInfo->AddRef();

	pMeshContainer->iNumBones = pMeshContainer->pSkinInfo->GetNumBones();

	pMeshContainer->pOffsetMatrices = new _matrix[pMeshContainer->iNumBones];

	/* �ٲ����ʴ� ����� offsetmatrix�� �ε�ÿ� �̸� �� �����ͳ�����. */
	for (DWORD i = 0; i < pMeshContainer->iNumBones; ++i)
	{
		pMeshContainer->pOffsetMatrices[i] = *pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i);
	}

	/* �������ø��� ó���Ѵ�. OffsetMatrix * CombinedTransformationMatrix */
	pMeshContainer->pRenderingMatrices = new _matrix[pMeshContainer->iNumBones];

	/* ���� �ѹ�. ���� �޽ÿ� ������ �ְ� �ִ� ������ CombinedTransformationMatrix����� �ּҸ� �������ѵѰ��̴�. */
	pMeshContainer->ppCombindTransformationMatrices = new _matrix * [pMeshContainer->iNumBones];

	LPD3DXMESH mesh;
	pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), pMeshContainer->MeshData.pMesh->GetFVF(), device, &mesh);
	pMeshContainer->pOriginalMesh.Attach(mesh);
	*ppNewMeshContainer = pMeshContainer;
	return S_OK;
}

auto HierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree)->HRESULT
{
	return S_OK;
}

auto HierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)->HRESULT
{
	return S_OK;
}

auto HierarchyLoader::Create(const ComPtr<IDirect3DDevice9>& device, const std::wstring& pMeshFilePath) -> std::shared_ptr<HierarchyLoader>
{
	auto instance=  std::make_shared<HierarchyLoader>(device);
	if (FAILED(instance->NativeConstruct(pMeshFilePath)))
		return nullptr;
	return instance;
}

auto HierarchyLoader::AllocateName(const char* pSourName, char** ppDestName) -> void
{
	const size_t	length = strlen(pSourName) + 1;

	*ppDestName = new char[length];
	strcpy_s(*ppDestName, length, pSourName);
}

