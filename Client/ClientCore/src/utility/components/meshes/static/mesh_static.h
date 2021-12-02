#pragma once
#include "src/utility/components/meshes/mesh.h"

class Shader;

class MeshStatic final : public Mesh
{
public:
	explicit MeshStatic(const ComPtr<IDirect3DDevice9>& device);
	virtual ~MeshStatic() = default;

public:
	auto NativeConstructPrototype(const std::wstring& filePath, const std::wstring& fileName)->HRESULT;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Clone(void* arg = nullptr)->std::shared_ptr<Component> override;

public:
	auto SetUpTextureOnShader(const std::shared_ptr<Shader>& shader, D3DXHANDLE parameter, MeshMaterialTexture::kType type, uint32_t materialIndex)->HRESULT;
	auto Render(int32_t index) const ->HRESULT;

	auto GetNumMaterials() const->uint32_t ;
public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device, const std::wstring& filePath, const std::wstring& fileName)->std::shared_ptr<MeshStatic>;

private:
	LPD3DXBUFFER						_adjacency = nullptr;
	LPD3DXBUFFER						_buffer_material = nullptr;
	DWORD								_num_materials = 0;
	ComPtr<ID3DXMesh>					_mesh = nullptr;
	std::vector<MeshMaterialTexture*>	_materials;

};

