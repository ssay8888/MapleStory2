#include "pch.h"
#include "mesh_dynamic.h"

#include <iostream>

#include "animation/animation.h"
#include "data_reader/data_reader_manager.h"
#include "loader/hierarchy_loader.h"
#include "src/utility/components/shader/shader.h"

MeshDynamic::MeshDynamic(const ComPtr<IDirect3DDevice9>& device):
	Mesh(device)
{
}

MeshDynamic::MeshDynamic(const MeshDynamic& rhs)
	: Mesh(rhs)
	, _pivot_matrix(rhs._pivot_matrix)
	, _root_frame(rhs._root_frame)
	, _animation(rhs._animation->Clone())
	, _mesh_containers(rhs._mesh_containers)
{
	const size_t iNumMeshContainer = _mesh_containers.size();

	for (size_t i = 0; i < iNumMeshContainer; ++i)
	{
		_mesh_containers[i]->MeshData.pMesh->AddRef();
	}
}

auto MeshDynamic::GetNumMeshContainer() const -> size_t
{
	return _mesh_containers.size();
}

auto MeshDynamic::GetNumMaterials(const uint32_t meshContainerIndex) const -> uint32_t
{
	return _mesh_containers[meshContainerIndex]->NumMaterials;
}

auto MeshDynamic::GetAnimation() const -> std::shared_ptr<Animation>
{
	return _animation;
}

auto MeshDynamic::GetBoneMatrixPointer(const char* pBoneName) const ->const  _matrix*
{
	const auto frame = static_cast<D3DxFrameDerived*>(D3DXFrameFind(_root_frame, pBoneName));
	return &frame->CombinedTransformationMatrix;
}

auto MeshDynamic::GetAnimationInfo() const -> std::shared_ptr<DataReaderManager::AnimationInfo>
{
	return _animation_info;
}

auto MeshDynamic::NativeConstructPrototype(const std::wstring& filePath, const std::wstring& fileName) -> HRESULT
{
	wchar_t		szFullPath[MAX_PATH] = TEXT("");

	lstrcpy(szFullPath, filePath.c_str());
	lstrcat(szFullPath, fileName.c_str());

	const std::shared_ptr<HierarchyLoader> hierarchyLoader = HierarchyLoader::Create(_graphic_device, filePath);

	LPD3DXANIMATIONCONTROLLER	animationController = nullptr;

	if (FAILED(D3DXLoadMeshHierarchyFromX(szFullPath, D3DXMESH_MANAGED, _graphic_device.Get(), hierarchyLoader.get(), nullptr, &_root_frame, &animationController)))
		return E_FAIL;

	_animation = Animation::Create(animationController);
	if (nullptr == _animation)
		return E_FAIL;


	D3DXMatrixIdentity(&_pivot_matrix);

	UpdateCombinedTransformationMatrices(_root_frame, _pivot_matrix);

	SetUpCombinedTransformationMatricesPointer(_root_frame);

	return S_OK;
}

HRESULT MeshDynamic::NativeConstruct(void* arg)
{
	if (arg)
	{
		_animation_info = *static_cast<std::shared_ptr<DataReaderManager::AnimationInfo>*>(arg);
	}
	return S_OK;
}

auto MeshDynamic::UpdateCombinedTransformationMatrices(const LPD3DXFRAME frame,
                                                       const _matrix parentCombinedTransformationMatrix) -> void
{
	const auto frameDerived = static_cast<D3DxFrameDerived*>(frame);
		
	frameDerived->CombinedTransformationMatrix = frameDerived->TransformationMatrix * parentCombinedTransformationMatrix;

	if (nullptr != frameDerived->pFrameFirstChild)
		UpdateCombinedTransformationMatrices(frameDerived->pFrameFirstChild, frameDerived->CombinedTransformationMatrix);

	if (nullptr != frameDerived->pFrameSibling)
		UpdateCombinedTransformationMatrices(frameDerived->pFrameSibling, parentCombinedTransformationMatrix);
}

auto MeshDynamic::SetUpCombinedTransformationMatricesPointer(const LPD3DXFRAME frame) -> void
{
	if (nullptr != frame->pMeshContainer)
	{
		auto pMeshContainer_Derived = static_cast<D3DXMeshContainerDerived*>(frame->pMeshContainer);

		_mesh_containers.push_back(pMeshContainer_Derived);

		for (uint32_t i = 0; i < pMeshContainer_Derived->iNumBones; ++i)
		{
			/* ���� �޽ÿ� ������ ��ġ�� ���� ��, i��° ���� �̸��� �������ֳ�. */
			const char* pBoneName = pMeshContainer_Derived->pSkinInfo->GetBoneName(i);

			const auto findFrame = static_cast<D3DxFrameDerived*>(D3DXFrameFind(_root_frame, pBoneName));

			pMeshContainer_Derived->ppCombindTransformationMatrices[i] = &findFrame->CombinedTransformationMatrix;
		}
	}

	if (nullptr != frame->pFrameFirstChild)
		SetUpCombinedTransformationMatricesPointer(frame->pFrameFirstChild);

	if (nullptr != frame->pFrameSibling)
		SetUpCombinedTransformationMatricesPointer(frame->pFrameSibling);
}

auto MeshDynamic::SetUpTextureOnShader(const std::shared_ptr<Shader>& shader, const D3DXHANDLE parameter, const MeshMaterialTexture::kType type,
                                        const uint32_t meshContainerIndex, const uint32_t materialIndex) -> HRESULT
{
	Microsoft::WRL::ComPtr<IDirect3DTexture9>			pTexture = nullptr;

	switch (type)
	{
	case MeshMaterialTexture::kType::kTypeDiffuse:
		pTexture = _mesh_containers[meshContainerIndex]->ppMaterialTextures[materialIndex]->diffuse_map;
		break;
	case MeshMaterialTexture::kType::kTypeNormal:
		pTexture = _mesh_containers[meshContainerIndex]->ppMaterialTextures[materialIndex]->normal_map;
		break;
	case MeshMaterialTexture::kType::kTypeSpecular:
		pTexture = _mesh_containers[meshContainerIndex]->ppMaterialTextures[materialIndex]->specular_map;
		break;
	default: 
		break;
	}

	if (FAILED(shader->SetUpTextureConstantTable(parameter, pTexture)))
		return E_FAIL;

	return S_OK;
}

auto MeshDynamic::UpdateSkinnedMesh(const uint32_t iMeshContainerIndex) -> HRESULT
{
	for (size_t i = 0; i < _mesh_containers[iMeshContainerIndex]->iNumBones; ++i)
	{
		_mesh_containers[iMeshContainerIndex]->pRenderingMatrices[i] = _mesh_containers[iMeshContainerIndex]->pOffsetMatrices[i] * *_mesh_containers[iMeshContainerIndex]->ppCombindTransformationMatrices[i];
	}

	void* pSour = nullptr, * pDest = nullptr;

	_mesh_containers[iMeshContainerIndex]->pOriginalMesh->LockVertexBuffer(0, &pSour);
	_mesh_containers[iMeshContainerIndex]->MeshData.pMesh->LockVertexBuffer(0, &pDest);

	_mesh_containers[iMeshContainerIndex]->pSkinInfo->UpdateSkinnedMesh(_mesh_containers[iMeshContainerIndex]->pRenderingMatrices, nullptr, pSour, pDest);

	_mesh_containers[iMeshContainerIndex]->pOriginalMesh->UnlockVertexBuffer();
	_mesh_containers[iMeshContainerIndex]->MeshData.pMesh->UnlockVertexBuffer();
	return S_OK;
}

auto MeshDynamic::Render(const uint32_t meshContainerIndex, const uint32_t materialIndex) -> HRESULT
{
	size_t		iNumMeshContainer = _mesh_containers.size();

	_mesh_containers[meshContainerIndex]->MeshData.pMesh->DrawSubset(materialIndex);

	return S_OK;
}

auto MeshDynamic::SetAnimationIndex(const uint32_t animIndex) const -> HRESULT
{
	if (nullptr == _animation)
		return E_FAIL;

	return _animation->SetAnimationIndex(animIndex);
}

auto MeshDynamic::ResetAnimation() const -> HRESULT
{
	if (nullptr == _animation)
		return E_FAIL;

	return _animation->ResetAnimation();
}

auto MeshDynamic::PlayAnimation(const double timeDelta) -> HRESULT
{
	if (nullptr == _animation)
		return E_FAIL;

	if (FAILED(_animation->PlayAnimation(timeDelta)))
		return E_FAIL;

	UpdateCombinedTransformationMatrices(_root_frame, _pivot_matrix);

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