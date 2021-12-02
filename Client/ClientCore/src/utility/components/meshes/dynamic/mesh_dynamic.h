#pragma once
#include "src/utility/components/meshes/mesh.h"

class MeshDynamic final : public Mesh
{
public:
	explicit MeshDynamic(const ComPtr<IDirect3DDevice9>& device);
	virtual ~MeshDynamic() = default;
public:
	virtual auto NativeConstructPrototype(const std::wstring& filePath, const std::wstring& fileName)->HRESULT;
	virtual auto NativeConstruct(void* arg)->HRESULT override;
private:
	LPD3DXFRAME				_root_frame = nullptr;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device, const std::wstring& filePath, const std::wstring& fileName)->std::shared_ptr<MeshDynamic>;
	virtual auto Clone(void* arg)->std::shared_ptr<Component> override;
};

