#pragma once
#include "src/utility/components/meshes/mesh.h"

class Shader;

class MeshDynamic final : public Mesh
{
public:
	explicit MeshDynamic(const ComPtr<IDirect3DDevice9>& device);
	explicit MeshDynamic(const MeshDynamic& rhs);
	virtual ~MeshDynamic() = default;
public:

	auto GetNumMeshContainer() const->size_t ;

	auto GetNumMaterials(uint32_t meshContainerIndex) const->uint32_t ;

public:
	virtual auto NativeConstructPrototype(const std::wstring& filePath, const std::wstring& fileName)->HRESULT;
	virtual auto NativeConstruct(void* arg)->HRESULT override;

public:
	auto UpdateCombinedTransformationMatrices(LPD3DXFRAME pFrame, _matrix ParentCombinedTransformationMatrix)->void;
	auto SetUpCombinedTransformationMatricesPointer(LPD3DXFRAME pFrame)->void;
	auto SetUpTextureOnShader(const std::shared_ptr<Shader>& shader, D3DXHANDLE parameter, MeshMaterialTexture::kType type, uint32_t meshContainerIndex, uint32_t materialIndex)->HRESULT;
	auto UpdateSkinnedMesh(uint32_t iMeshContainerIndex)->HRESULT;
	auto Render(uint32_t meshContainerIndex, uint32_t materialIndex)->HRESULT;

private:
	LPD3DXFRAME				_root_frame = nullptr;
	_matrix					_pivot_matrix;
	std::vector<D3DXMeshContainerDerived*>		_mesh_containers;
public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device, const std::wstring& filePath, const std::wstring& fileName)->std::shared_ptr<MeshDynamic>;
	virtual auto Clone(void* arg)->std::shared_ptr<Component> override;
};

