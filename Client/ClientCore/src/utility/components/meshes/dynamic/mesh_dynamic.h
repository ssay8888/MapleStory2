#pragma once
#include "data_reader/data_reader_manager.h"
#include "src/utility/components/meshes/mesh.h"

class HierarchyLoader;
class Shader;
class Animation;

class MeshDynamic final : public Mesh
{
public:
	explicit MeshDynamic(const ComPtr<IDirect3DDevice9>& device);
	explicit MeshDynamic(const MeshDynamic& rhs);
	virtual ~MeshDynamic();
public:

	auto GetNumMeshContainer() const->size_t ;

	auto GetNumMaterials(uint32_t meshContainerIndex) const->uint32_t ;
	auto GetAnimation() const->std::shared_ptr<Animation>;

	auto GetBoneMatrixPointer(const char* pBoneName) const ->const _matrix* ;

	auto GetAnimationInfo() const ->std::shared_ptr<DataReaderManager::AnimationInfo>;

	auto GetRootFrame()->LPD3DXFRAME;

	auto GetMeshContainer() const ->std::vector<D3DXMeshContainerDerived*>;

	auto GetItemId() const->int32_t;
public:
	virtual auto NativeConstructPrototype(const std::wstring& filePath, const std::wstring& fileName, int32_t itemId)->HRESULT;
	virtual auto NativeConstruct(void* arg)->HRESULT override;

public:
	auto UpdateCombinedTransformationMatrices(LPD3DXFRAME frame, _matrix parentCombinedTransformationMatrix)->void;
	auto TargetCombinedTransformationMatrices(LPD3DXFRAME frame, LPD3DXFRAME targetFrame)->void;
	auto SetUpCombinedTransformationMatricesPointer(LPD3DXFRAME frame)->void;
	void SetUpCloneCombinedTransformationMatricesPointer(LPD3DXFRAME pFrame, uint32_t& index);
	auto SetUpTextureOnShader(const std::shared_ptr<Shader>& shader, D3DXHANDLE parameter, MeshMaterialTexture::kType type, uint32_t meshContainerIndex, uint32_t materialIndex)->HRESULT;
	auto UpdateSkinnedMesh(uint32_t iMeshContainerIndex)->HRESULT;
	auto ChangeSkinnedMesh(std::shared_ptr<MeshDynamic> target, std::string remove)->HRESULT;
	auto ChangeFaceTexture(ComPtr<IDirect3DTexture9> texture)->void;
	auto SetSkinnedMesh(const D3DXMeshContainerDerived* matrix)->void;
	auto FindSkinnedMesh(const std::string& name)->D3DXMeshContainerDerived*;
	auto Render(uint32_t meshContainerIndex, uint32_t materialIndex)->HRESULT;
	auto SetAnimationIndex(uint32_t animIndex) const ->HRESULT;
	auto ResetAnimation() const ->HRESULT;
	auto PlayAnimation(double timeDelta)->HRESULT;

private:
	HRESULT CloneFrame(D3DxFrameDerived* pOriginalFrame);
	void CloneFrame(const D3DxFrameDerived* pOriginalFrame, D3DxFrameDerived* pOut);

private:
	_matrix												_pivot_matrix;
	LPD3DXFRAME											_root_frame = nullptr;
	std::shared_ptr<Animation>							_animation = nullptr;
	std::vector<_matrix**>								_combined_transfromation_matrix_pointers;
	std::vector<_matrix**>								_origin_combined_transfromation_matrix_pointers;
	std::vector<D3DXMeshContainerDerived*>				_mesh_containers;
	std::vector<D3DXMeshContainerDerived*>				_origin_mesh_containers;
	std::shared_ptr<DataReaderManager::AnimationInfo>	_animation_info;
	std::shared_ptr<HierarchyLoader>					_loader;

	int32_t												_item_id = 0;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device, const std::wstring& filePath, const std::wstring& fileName, int32_t itemId = 0)->std::shared_ptr<MeshDynamic>;
	virtual auto Clone(void* arg)->std::shared_ptr<Component> override;
};

