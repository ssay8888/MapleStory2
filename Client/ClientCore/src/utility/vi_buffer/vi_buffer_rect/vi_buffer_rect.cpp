#include "pch.h"
#include "vi_buffer_rect.h"

ViBufferRect::ViBufferRect(const ComPtr<IDirect3DDevice9>& graphicDevice):
	ViBuffer(graphicDevice)
{
}
HRESULT ViBufferRect::NativeConstruct(void* arg)
{
	return S_OK;
}

HRESULT ViBufferRect::NativeConstructPrototype()
{
#pragma region VERTICES
	_num_vertices = 4;
	_stride = sizeof(VtxTexture);
	_vb_usage = 0;
	_fvf = D3DFVF_XYZ | D3DFVF_TEX1;
	_primitive_type = D3DPT_TRIANGLELIST;
#pragma endregion


#pragma region INDICES
	_num_primitive = 2;
	_num_vertices_per_primitive = 3;
	_ib_usage = 0;

	/* 2 or 4 */
	_one_index_size = 2;
#pragma endregion

	if (FAILED(ViBuffer::NativeConstructPrototype()))
		return E_FAIL;

	VtxTexture* vertices = nullptr;
	
	_vb->Lock(0, 0, reinterpret_cast<void**>(&vertices), 0);


	vertices[0].v_position = _vertices_pos[0] = _float3(-0.5f, 0.5f, 0.f);
	vertices[0].v_tex_uv	= _float2(0.f, 0.f);

	vertices[1].v_position = _vertices_pos[1] = _float3(0.5f, 0.5f, 0.f);
	vertices[1].v_tex_uv	= _float2(1.f, 0.f);

	vertices[2].v_position = _vertices_pos[2] = _float3(0.5f, -0.5f, 0.f);
	vertices[2].v_tex_uv	= _float2(1.f, 1.f);

	vertices[3].v_position = _vertices_pos[3] = _float3(-0.5f, -0.5f, 0.f);
	vertices[3].v_tex_uv	= _float2(0.f, 1.f);

	_vb->Unlock();


	uint16_t* indices = nullptr;

	_ib->Lock(0, 0, reinterpret_cast<void**>(&indices), 0);

	static_cast<uint16_t*>(_indices)[0] = indices[0] = 0;
	static_cast<uint16_t*>(_indices)[1] = indices[1] = 1;
	static_cast<uint16_t*>(_indices)[2] = indices[2] = 2;
	  			
	static_cast<uint16_t*>(_indices)[3] = indices[3] = 0;
	static_cast<uint16_t*>(_indices)[4] = indices[4] = 2;
	static_cast<uint16_t*>(_indices)[5] = indices[5] = 3;

	_ib->Unlock();

	return ViBuffer::NativeConstructPrototype();
}

auto ViBufferRect::Clone(void* pArg) -> std::shared_ptr<Component>
{
	auto instance = std::make_shared<ViBufferRect>(*this);

	if (FAILED(instance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating VIBufferRect");
		return nullptr;
	}
	return instance;
}
