#include "pch.h"
#include "mesh_dynamic.h"

#include <iostream>

#include "animation/animation.h"
#include "data_reader/data_reader_manager.h"
#include "loader/hierarchy_loader.h"
#include "src/utility/components/shader/shader.h"

MeshDynamic::MeshDynamic(const ComPtr<IDirect3DDevice9>& device) :
	Mesh(device)
{
}
 
MeshDynamic::MeshDynamic(const MeshDynamic& rhs)
	: Mesh(rhs)
	, _pivot_matrix(rhs._pivot_matrix)
	, _root_frame(rhs._root_frame)
	, _animation(rhs._animation->Clone())
	, _mesh_containers(rhs._mesh_containers)
	, _loader(rhs._loader)
{
	const size_t iNumMeshContainer = _mesh_containers.size();

	for (size_t i = 0; i < iNumMeshContainer; ++i)
	{
		_mesh_containers[i]->MeshData.pMesh->AddRef();
	}
}

MeshDynamic::~MeshDynamic()
{
	Mesh::~Mesh();


	const size_t numMeshContainer = _mesh_containers.size();

	for (size_t i = 0; i < numMeshContainer; ++i)
	{
		_mesh_containers[i]->MeshData.pMesh->Release();
	}

	_mesh_containers.clear();

	_loader->DestroyFrame(_root_frame);

	for (auto& ppCombinedMatrixPointers : _origin_combined_transfromation_matrix_pointers)
	{
		if (ppCombinedMatrixPointers)
		{
			delete[] ppCombinedMatrixPointers;
			ppCombinedMatrixPointers = nullptr;
		}
	}

	_origin_combined_transfromation_matrix_pointers.clear();

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

auto MeshDynamic::GetRootFrame() -> LPD3DXFRAME
{
	return _root_frame;
}

auto MeshDynamic::GetMeshContainer() const -> std::vector<D3DXMeshContainerDerived*>
{
	return _mesh_containers;
}

auto MeshDynamic::GetItemId() const -> int32_t
{
	return _item_id;
}

auto MeshDynamic::NativeConstructPrototype(const std::wstring& filePath, const std::wstring& fileName, int32_t itemId)->HRESULT
{
	_item_id = itemId;
	wchar_t		szFullPath[MAX_PATH] = TEXT("");

	lstrcpy(szFullPath, filePath.c_str());
	lstrcat(szFullPath, fileName.c_str());

	_loader = HierarchyLoader::Create(_graphic_device, filePath);

	LPD3DXANIMATIONCONTROLLER	animationController = nullptr;

	if (FAILED(D3DXLoadMeshHierarchyFromX(szFullPath, D3DXMESH_MANAGED, _graphic_device.Get(), _loader.get(), nullptr, &_root_frame, &animationController)))
		return E_FAIL;

	_animation = Animation::Create(animationController);
	if (nullptr == _animation)
		return E_FAIL;


	D3DXMatrixIdentity(&_pivot_matrix);

	UpdateCombinedTransformationMatrices(_root_frame, _pivot_matrix);

	SetUpCombinedTransformationMatricesPointer(_root_frame);
	_origin_mesh_containers = _mesh_containers;
	return S_OK;
}

HRESULT MeshDynamic::NativeConstruct(void* arg)
{
	if (arg)
	{
		_animation_info = *static_cast<std::shared_ptr<DataReaderManager::AnimationInfo>*>(arg);
	}
	_combined_transfromation_matrix_pointers.resize(_mesh_containers.size());
	_origin_combined_transfromation_matrix_pointers.resize(_mesh_containers.size());

	/* 뼈를 복제하낟. */
	auto originalFrame = static_cast<D3DxFrameDerived*>(_root_frame);

	/* m_pRootFrame에 원본뼏르을을 복샇ㅇ령 담는다.  */
	if (FAILED(CloneFrame(originalFrame)))
		return E_FAIL;

	for (size_t i = 0; i < _mesh_containers.size(); ++i)
	{
		_combined_transfromation_matrix_pointers[i] = new _matrix*[_mesh_containers[i]->iNumBones];
		_origin_combined_transfromation_matrix_pointers[i] = _combined_transfromation_matrix_pointers[i];
	}

	uint32_t index = 0;
	SetUpCloneCombinedTransformationMatricesPointer(_root_frame, index);
	_animation->BindFrames(static_cast<D3DxFrameDerived*>(_root_frame));
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

auto MeshDynamic::TargetCombinedTransformationMatrices(std::shared_ptr<MeshDynamic> frame,
	std::shared_ptr<MeshDynamic> targetFrame) -> void
{
	auto targetMeshContainer = static_cast<D3DXMeshContainerDerived*>(targetFrame->GetRootFrame()->pMeshContainer);
	auto meshContainer = static_cast<D3DXMeshContainerDerived*>(frame->GetRootFrame()->pMeshContainer);


	if (meshContainer)
	{
		for (uint32_t j = 0; j < meshContainer->iNumBones; ++j)
		{
			const std::string BoneName = meshContainer->pSkinInfo->GetBoneName(j);
			{
				const auto findFrame = static_cast<D3DxFrameDerived*>(D3DXFrameFind(targetFrame->GetRootFrame(), BoneName.c_str()));
				if (findFrame)
				{
					meshContainer->ppCombindTransformationMatrices[j] = &findFrame->CombinedTransformationMatrix;
				}
			}
		}
	}
	if (nullptr != frame->GetRootFrame()->pFrameFirstChild)
		TargetCombinedTransformationMatrices(frame->GetRootFrame()->pFrameFirstChild, targetFrame->GetRootFrame());

	if (nullptr != frame->GetRootFrame()->pFrameSibling)
		TargetCombinedTransformationMatrices(frame->GetRootFrame()->pFrameSibling, targetFrame->GetRootFrame());
}

auto MeshDynamic::TargetCombinedTransformationMatrices(LPD3DXFRAME frame, LPD3DXFRAME targetFrame) -> void
{
	auto targetMeshContainer = static_cast<D3DXMeshContainerDerived*>(targetFrame->pMeshContainer);
	auto meshContainer = static_cast<D3DXMeshContainerDerived*>(frame->pMeshContainer);


	if (meshContainer)
	{
		for (uint32_t j = 0; j < meshContainer->iNumBones; ++j)
		{
			const std::string BoneName = meshContainer->pSkinInfo->GetBoneName(j);
			{
				const auto findFrame = static_cast<D3DxFrameDerived*>(D3DXFrameFind(targetFrame, BoneName.c_str()));
				if (findFrame)
				{
					meshContainer->ppCombindTransformationMatrices[j] = &findFrame->CombinedTransformationMatrix;
				}
			}
		}
	}
	if (nullptr != frame->pFrameFirstChild)
		TargetCombinedTransformationMatrices(frame->pFrameFirstChild, targetFrame);

	if (nullptr != frame->pFrameSibling)
		TargetCombinedTransformationMatrices(frame->pFrameSibling, targetFrame);
}

auto MeshDynamic::SetUpCombinedTransformationMatricesPointer(const LPD3DXFRAME frame) -> void
{
	if (nullptr != frame->pMeshContainer)
	{
		auto pMeshContainer_Derived = static_cast<D3DXMeshContainerDerived*>(frame->pMeshContainer);

		_mesh_containers.push_back(pMeshContainer_Derived);

		for (uint32_t i = 0; i < pMeshContainer_Derived->iNumBones; ++i)
		{
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

void MeshDynamic::SetUpCloneCombinedTransformationMatricesPointer(LPD3DXFRAME pFrame, uint32_t& index)
{
	if (nullptr != pFrame->pMeshContainer)
	{
		auto pMeshContainer_Derived = static_cast<D3DXMeshContainerDerived*>(pFrame->pMeshContainer);

		for (uint32_t i = 0; i < pMeshContainer_Derived->iNumBones; ++i)
		{
			/* 현재 메시에 영향을 미치는 뼈들 중, i번째 뼈의 이름을 리턴해주낟. */
			const char* pBoneName = pMeshContainer_Derived->pSkinInfo->GetBoneName(i);

			auto pFindFrame = static_cast<D3DxFrameDerived*>(D3DXFrameFind(_root_frame, pBoneName));

			_combined_transfromation_matrix_pointers[index][i] = &pFindFrame->CombinedTransformationMatrix;
			_origin_combined_transfromation_matrix_pointers[index][i] = _combined_transfromation_matrix_pointers[index][i];
			// pMeshContainer_Derived->ppCombindTransformationMatrices[i] = &pFindFrame->CombinedTransformationMatrix;
		}

		++index;
	}



	if (nullptr != pFrame->pFrameFirstChild)
		SetUpCloneCombinedTransformationMatricesPointer(pFrame->pFrameFirstChild, index);

	if (nullptr != pFrame->pFrameSibling)
		SetUpCloneCombinedTransformationMatricesPointer(pFrame->pFrameSibling, index);
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
		//std::cout << _mesh_containers[iMeshContainerIndex]->Name << "/" << _mesh_containers[iMeshContainerIndex]->iNumBones << std::endl;
		//auto a = _combined_transfromation_matrix_pointers[iMeshContainerIndex][i];
		_mesh_containers[iMeshContainerIndex]->pRenderingMatrices[i] = _mesh_containers[iMeshContainerIndex]->pOffsetMatrices[i] * 
			*_combined_transfromation_matrix_pointers[iMeshContainerIndex][i];
	}

	void* pSour = nullptr, * pDest = nullptr;
	_mesh_containers[iMeshContainerIndex]->pOriginalMesh->LockVertexBuffer(0, &pSour);
	_mesh_containers[iMeshContainerIndex]->MeshData.pMesh->LockVertexBuffer(0, &pDest);

	_mesh_containers[iMeshContainerIndex]->pSkinInfo->UpdateSkinnedMesh(_mesh_containers[iMeshContainerIndex]->pRenderingMatrices, nullptr, pSour, pDest);

	_mesh_containers[iMeshContainerIndex]->pOriginalMesh->UnlockVertexBuffer();
	_mesh_containers[iMeshContainerIndex]->MeshData.pMesh->UnlockVertexBuffer();
	return S_OK;
}

auto MeshDynamic::ChangeSkinnedMesh(const std::shared_ptr<MeshDynamic> target, const std::string remove) -> HRESULT
{
	auto container = target->GetMeshContainer();
	std::vector<int32_t> indexList;
	int32_t index = 0;
	for (auto iter = container.begin(); iter != container.end(); ++iter)
	{
		//remove가 포함된 모든 매쉬를 지운다.
		for (auto iter = _mesh_containers.begin(); iter != _mesh_containers.end();)
		{
			std::string name((*iter)->Name);
			auto pos = name.find(remove);

			if (pos != std::string::npos)
			{
				iter = _mesh_containers.erase(iter);
				indexList.push_back(index);
				++index;
				continue;
			}
			++iter;
			++index;
		}
	}
	auto meshs = target->GetMeshContainer();
	_mesh_containers.insert(_mesh_containers.end(), meshs.begin(), meshs.end());
	index = 0;
	for (auto iter = _combined_transfromation_matrix_pointers.begin(); iter != _combined_transfromation_matrix_pointers.end();)
	{
		bool isErase = false;
		for (auto num : indexList)
		{
			if (num == index)
			{
				iter = _combined_transfromation_matrix_pointers.erase(iter);
				index++;
				isErase = true;
				break;
			}
		}
		if (!isErase)
		{
			++iter;
			index++;
		}
	}
	_combined_transfromation_matrix_pointers.insert(_combined_transfromation_matrix_pointers.end(), 
		target->_origin_combined_transfromation_matrix_pointers.begin(), 
		target->_origin_combined_transfromation_matrix_pointers.end());
	//bool isSkin = false;
	//for (auto iter = _mesh_containers.begin(); iter != _mesh_containers.end(); ++iter)
	//{
	//	//컨테이너에 skin이없다면 추가한다.
	//	std::string Name((*iter)->Name);
	//	auto skin = Name.find("Skin");
	//	if (skin != std::string::npos)
	//	{
	//		isSkin = true;
	//	}
	//}
	//if (!isSkin)
	//{
	//	for (auto iter = _origin_mesh_containers.begin(); iter != _origin_mesh_containers.end(); ++iter)
	//	{
	//		//컨테이너에 skin이없다면 추가한다.
	//		std::string Name((*iter)->Name);
	//		auto skin = Name.find("Skin");
	//		if (skin != std::string::npos)
	//		{
	//			isSkin = true;
	//			_mesh_containers.push_back(*iter);
	//		}
	//	}
	//}
	//auto container = target->GetMeshContainer();
	//for (auto iter = container.begin(); iter != container.end(); ++iter)
	//{
	//	//타겟컨테이너에 Skin이 포함된 메시가 들어있다면, 모든 관련된 메시를 삭제한다.
	//	std::string Name((*iter)->Name);
	//	std::string RemoveCpy(remove);
	//	auto pos = Name.find(RemoveCpy.append("Skin"));
	//	if (pos != std::string::npos || Name == remove)
	//	{
	//		for (auto iter = _mesh_containers.begin(); iter != _mesh_containers.end();)
	//		{
	//			std::string name((*iter)->Name);
	//			auto pos = name.find(remove);

	//			if (pos != std::string::npos)
	//			{
	//				iter = _mesh_containers.erase(iter);
	//				continue;
	//			}
	//			++iter;
	//		}
	//	}
	//}
	//for (auto iter = _mesh_containers.begin(); iter != _mesh_containers.end();)
	//{
	//	//내 컨테이너에 Skin을 제외한 관련된 삭제할타겟이 포함된 텍스트가있으면 메시를 삭제함.
	//	std::string Name((*iter)->Name);
	//	auto pos = Name.find(remove, 0);
	//	auto skin = Name.find("Skin");
	//	if (skin == std::string::npos && pos != std::string::npos && pos <= 3)
	//	{
	//		if (strcmp(remove.c_str(), "CL_"))
	//		{
	//			iter = _mesh_containers.erase(iter);
	//			continue;
	//		}
	//	}
	//	++iter;
	//}
	//auto meshs = target->GetMeshContainer();
	//_mesh_containers.insert(_mesh_containers.end(), meshs.begin(), meshs.end());
	return S_OK;
}

auto MeshDynamic::ChangeFaceTexture(ComPtr<IDirect3DTexture9> texture)->void
{
	//remove가 포함된 모든 매쉬를 지운다.
	for (auto iter = _mesh_containers.begin(); iter != _mesh_containers.end(); ++iter)
	{
		std::string name((*iter)->Name);

		if (name == "FA_")
		{
			(*iter)->ppMaterialTextures[0]->diffuse_map = texture;
			break;;
		}
	}

}

auto MeshDynamic::SetSkinnedMesh(const D3DXMeshContainerDerived* container)->void
{
	for (const auto& mesh : _mesh_containers)
	{
		void* pSour = nullptr, * pDest = nullptr;
		mesh->pOriginalMesh->LockVertexBuffer(0, &pSour);
		mesh->MeshData.pMesh->LockVertexBuffer(0, &pDest);

		mesh->pSkinInfo->UpdateSkinnedMesh(container->pRenderingMatrices, nullptr, pSour, pDest);

		mesh->pOriginalMesh->UnlockVertexBuffer();
		mesh->MeshData.pMesh->UnlockVertexBuffer();
	}
}

auto MeshDynamic::FindSkinnedMesh(const std::string& name) -> D3DXMeshContainerDerived*
{
	int32_t index;
	for (index = 0; index < _mesh_containers.size(); ++index)
	{
		std::cout << _mesh_containers[index]->Name << "/" << _mesh_containers[index]->iNumBones << std::endl;
		if (_mesh_containers[index]->Name == name)
		{
			return _mesh_containers[index];
		}
	}
	return nullptr;
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

HRESULT MeshDynamic::CloneFrame(D3DxFrameDerived* pOriginalFrame)
{

	_root_frame = new D3DxFrameDerived;
	memcpy(_root_frame, pOriginalFrame, sizeof(D3DxFrameDerived));

	static_cast<D3DxFrameDerived*>(_root_frame)->is_cloned = true;

	if (nullptr != pOriginalFrame->pFrameFirstChild)
	{
		_root_frame->pFrameFirstChild = new D3DxFrameDerived;
		CloneFrame(static_cast<D3DxFrameDerived*>(pOriginalFrame->pFrameFirstChild), static_cast<D3DxFrameDerived*>(_root_frame->pFrameFirstChild));
	}

	if (nullptr != pOriginalFrame->pFrameSibling)
	{
		_root_frame->pFrameSibling = new D3DxFrameDerived;
		CloneFrame(static_cast<D3DxFrameDerived*>(pOriginalFrame->pFrameSibling), static_cast<D3DxFrameDerived*>(_root_frame->pFrameSibling));
	}

	return S_OK;
}

void MeshDynamic::CloneFrame(const D3DxFrameDerived* pOriginalFrame, D3DxFrameDerived* pOut)
{
	memcpy(pOut, pOriginalFrame, sizeof(D3DxFrameDerived));

	pOut->is_cloned = true;

	if (nullptr != pOriginalFrame->pFrameFirstChild)
	{
		pOut->pFrameFirstChild = new D3DxFrameDerived;
		CloneFrame(static_cast<D3DxFrameDerived*>(pOriginalFrame->pFrameFirstChild), static_cast<D3DxFrameDerived*>(pOut->pFrameFirstChild));
	}

	if (nullptr != pOriginalFrame->pFrameSibling)
	{
		pOut->pFrameSibling = new D3DxFrameDerived;
		CloneFrame(static_cast<D3DxFrameDerived*>(pOriginalFrame->pFrameSibling), static_cast<D3DxFrameDerived*>(pOut->pFrameSibling));
	}
}

auto MeshDynamic::Create(const ComPtr<IDirect3DDevice9>& device, const std::wstring& filePath,
                         const std::wstring& fileName, int32_t itemId) -> std::shared_ptr<MeshDynamic>
{
	auto pInstance = std::make_shared<MeshDynamic>(device);


	if (FAILED(pInstance->NativeConstructPrototype(filePath, fileName, itemId)))
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
