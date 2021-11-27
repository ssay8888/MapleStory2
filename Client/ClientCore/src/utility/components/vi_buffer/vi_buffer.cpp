#include "pch.h"
#include "vi_buffer.h"

ViBuffer::ViBuffer(const ComPtr<IDirect3DDevice9>& graphicDevice):
	Component(graphicDevice)
{
}

ViBuffer::~ViBuffer()
{
	if (false == _is_clone)
	{
		delete[] _vertices_pos;
		delete[] _indices;
	}
}

auto ViBuffer::GetNumPolygons() const -> uint32_t
{
	return _num_primitive;
}

auto ViBuffer::GetPolygonVertexPositions(const uint32_t polygonIndex, _float3* points) const -> void
{
	const uint32_t index = polygonIndex * 3;

	if (_one_index_size == 2)
	{
		points[0] = _vertices_pos[static_cast<uint16_t*>(_indices)[index]];
		points[1] = _vertices_pos[static_cast<uint16_t*>(_indices)[index + 1]];
		points[2] = _vertices_pos[static_cast<uint16_t*>(_indices)[index + 2]];

	}
	else
	{
		points[0] = _vertices_pos[static_cast<uint64_t*>(_indices)[index]];
		points[1] = _vertices_pos[static_cast<uint64_t*>(_indices)[index + 1]];
		points[2] = _vertices_pos[static_cast<uint64_t*>(_indices)[index + 2]];
	}
}

HRESULT ViBuffer::NativeConstructPrototype()
{
	if (nullptr == _graphic_device)
		return E_FAIL;

	if (FAILED(_graphic_device->CreateVertexBuffer(_stride * _num_vertices, _vb_usage, _fvf, D3DPOOL_MANAGED, &_vb, nullptr)))
			return E_FAIL;
	
	if (FAILED(_graphic_device->CreateIndexBuffer(_one_index_size * _num_primitive * _num_vertices_per_primitive, _ib_usage, _one_index_size == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_MANAGED, &_ib, nullptr)))
		return E_FAIL;

	_vertices_pos = new _float3[_num_vertices];

	const auto arraySize = _num_primitive * 3;
	if (_one_index_size == 2)
	{
		_indices = new uint16_t[arraySize];
	}
	else
	{
		_indices = new uint32_t[arraySize];
	}

	return S_OK;
}

HRESULT ViBuffer::NativeConstruct(void* arg)
{
	return S_OK;
}

auto ViBuffer::RenderViBuffer() -> HRESULT
{
	_graphic_device->SetStreamSource(0, _vb.Get(), 0, _stride);
	_graphic_device->SetFVF(_fvf);
	_graphic_device->SetIndices(_ib.Get());
	_graphic_device->DrawIndexedPrimitive(_primitive_type, 0, 0, _num_vertices, 0, _num_primitive);
	return S_OK;
}