#include "pch.h"
#include "vi_buffer_cube.h"

ViBufferCube::ViBufferCube(const ComPtr<IDirect3DDevice9>& graphicDevice):
	ViBuffer(graphicDevice)
{
}

HRESULT ViBufferCube::NativeConstructPrototype()
{
#pragma region VERTICES
	_num_vertices = 8;
	_stride = sizeof(TagVertexCubeTexture);
	_vb_usage = 0;
	_fvf = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
	_primitive_type = D3DPT_TRIANGLELIST;
#pragma endregion

#pragma region INDICES
	_num_primitive = 12;
	_num_vertices_per_primitive = 3;
	_ib_usage = 0;

	/* 2 or 4 */
	_one_index_size = 2;
#pragma endregion

	if (FAILED(ViBuffer::NativeConstructPrototype()))
		return E_FAIL;

	TagVertexCubeTexture* pVertices = nullptr;

	_vb->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);


	/* ¾Õ¸é */
	pVertices[0].v_position = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].v_tex_uv = pVertices[0].v_position;
	pVertices[1].v_position = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].v_tex_uv = pVertices[1].v_position;
	pVertices[2].v_position = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].v_tex_uv = pVertices[2].v_position;
	pVertices[3].v_position = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].v_tex_uv = pVertices[3].v_position;

	/* µÞ¸é */
	pVertices[4].v_position = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].v_tex_uv = pVertices[4].v_position;
	pVertices[5].v_position = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].v_tex_uv = pVertices[5].v_position;
	pVertices[6].v_position = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].v_tex_uv = pVertices[6].v_position;
	pVertices[7].v_position = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].v_tex_uv = pVertices[7].v_position;

	_vb->Unlock();


	uint16_t* pIndices = nullptr;

	_ib->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	/* +x */
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

	/* -x */
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;

	/* +y */
	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

	/* -y */
	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

	/* +z */
	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;

	/* -z */
	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;


	_ib->Unlock();
	return S_OK;
}

HRESULT ViBufferCube::NativeConstruct(void* arg)
{
	return S_OK;
}

std::shared_ptr<ViBufferCube> ViBufferCube::Create(const ComPtr<IDirect3DDevice9>& graphicDevice)
{
	auto instance = std::make_shared<ViBufferCube>(graphicDevice);

	if (FAILED(instance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating CVIBufferCube");
		return nullptr;
	}
	return instance;
}

std::shared_ptr<Component> ViBufferCube::Clone(void* arg)
{
	auto instance = std::make_shared<ViBufferCube>(*this);

	if (FAILED(instance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone CVIBufferCube");
		return nullptr;
	}
	return instance;
}
