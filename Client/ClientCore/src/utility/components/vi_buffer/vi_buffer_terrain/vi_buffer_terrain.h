#pragma once
#include "src/utility/components/vi_buffer/vi_buffer.h"

class ViBufferTerrain : public ViBuffer
{
public:
	explicit ViBufferTerrain(const ComPtr<IDirect3DDevice9>& graphicDevice);
	virtual ~ViBufferTerrain() = default;
public:
	auto GetPlane(const _float3& vPosition) const ->D3DXPLANE;

public:
	virtual auto NativeConstructPrototype(int32_t numVerticesX, int32_t numVerticesZ, float interval)->HRESULT;
	virtual auto NativeConstructPrototype(const std::wstring& heightMapPath, float interval)->HRESULT;
	virtual auto NativeConstruct(void* arg)->HRESULT override;

private:
	float			_interval = 0.f;
	uint32_t		_num_vertices_x = 0;
	uint32_t		_num_vertices_z = 0;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& graphicDevice, uint32_t iNumVerticesX, uint32_t numVerticesZ, float interval = 1.f)->std::shared_ptr<ViBufferTerrain>;
	static auto Create(const ComPtr<IDirect3DDevice9>& graphicDevice, const std::wstring& heightMapPath, float interval = 1.f)->std::shared_ptr<ViBufferTerrain>;
	auto Clone(void* pArg = nullptr)->std::shared_ptr<Component> override;
};

