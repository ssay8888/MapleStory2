#pragma once
#include "src/utility/components/component.h"

class ViBuffer : public Component
{
protected:
	explicit ViBuffer(const ComPtr<IDirect3DDevice9>& graphicDevice);
	virtual ~ViBuffer();

public:
	auto GetNumPolygons() const->uint32_t;
	auto GetPolygonVertexPositions(uint32_t polygonIndex, _float3* points) const->void;

	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	virtual auto RenderViBuffer()->HRESULT;
	auto Clone(void* pArg)->std::shared_ptr<Component> override = 0;


protected:
	ComPtr<IDirect3DVertexBuffer9>		_vb = nullptr;
	ComPtr<IDirect3DIndexBuffer9>		_ib = nullptr;

	int32_t		_stride = 0;
	int32_t		_num_vertices = 0;
	DWORD		_vb_usage = 0;
	DWORD		_fvf = 0;
	_float3*	_vertices_pos = nullptr;


	uint32_t	_num_primitive = 0;
	int32_t		_num_vertices_per_primitive = 0;
	int32_t		_one_index_size = 0;
	DWORD		_ib_usage = 0;
	void*		_indices = nullptr;

	D3DPRIMITIVETYPE	_primitive_type = static_cast<D3DPRIMITIVETYPE>(0);
};

