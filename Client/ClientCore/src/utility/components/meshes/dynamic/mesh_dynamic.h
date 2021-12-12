#pragma once
#include "data_reader/data_reader_manager.h"
#include "src/utility/components/meshes/mesh.h"

class Shader;
class Animation;

class MeshDynamic final : public Mesh
{
public:
	explicit MeshDynamic(const ComPtr<IDirect3DDevice9>& device);
	explicit MeshDynamic(const MeshDynamic& rhs);
	virtual ~MeshDynamic() = default;
public:

	auto GetNumMeshContainer() const->size_t ;

	auto GetNumMaterials(uint32_t meshContainerIndex) const->uint32_t ;
	auto GetAnimation() const->std::shared_ptr<Animation>;

	auto GetBoneMatrixPointer(const char* pBoneName) const ->const _matrix* ;

	auto GetAnimationInfo() const ->std::shared_ptr<DataReaderManager::AnimationInfo>;
public:
	virtual auto NativeConstructPrototype(const std::wstring& filePath, const std::wstring& fileName)->HRESULT;
	virtual auto NativeConstruct(void* arg)->HRESULT override;

public:
	auto UpdateCombinedTransformationMatrices(LPD3DXFRAME frame, _matrix parentCombinedTransformationMatrix)->void;
	auto SetUpCombinedTransformationMatricesPointer(LPD3DXFRAME frame)->void;
	auto SetUpTextureOnShader(const std::shared_ptr<Shader>& shader, D3DXHANDLE parameter, MeshMaterialTexture::kType type, uint32_t meshContainerIndex, uint32_t materialIndex)->HRESULT;
	auto UpdateSkinnedMesh(uint32_t iMeshContainerIndex)->HRESULT;
	auto Render(uint32_t meshContainerIndex, uint32_t materialIndex)->HRESULT;
	auto SetAnimationIndex(uint32_t animIndex) const ->HRESULT;
	auto ResetAnimation() const ->HRESULT;
	auto PlayAnimation(double timeDelta)->HRESULT;

private:
	_matrix										_pivot_matrix;
	LPD3DXFRAME									_root_frame = nullptr;
	std::shared_ptr<Animation>					_animation = nullptr;
	std::vector<D3DXMeshContainerDerived*>		_mesh_containers;
	std::shared_ptr<DataReaderManager::AnimationInfo> _animation_info;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device, const std::wstring& filePath, const std::wstring& fileName)->std::shared_ptr<MeshDynamic>;
	virtual auto Clone(void* arg)->std::shared_ptr<Component> override;
};

