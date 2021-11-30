#pragma once
#include "src/utility/components/meshes/mesh.h"

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
	auto Render()->HRESULT;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device, const std::wstring& filePath, const std::wstring& fileName)->std::shared_ptr<MeshStatic>;

private:
	LPD3DXBUFFER			_adjacency = nullptr;
	LPD3DXBUFFER			_materials = nullptr;
	DWORD					_num_materials = 0;
	ComPtr<ID3DXMesh>		_mesh = nullptr;

};

