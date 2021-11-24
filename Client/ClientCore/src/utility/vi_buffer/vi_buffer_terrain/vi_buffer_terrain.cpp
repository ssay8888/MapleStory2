#include "pch.h"
#include "vi_buffer_terrain.h"

ViBufferTerrain::ViBufferTerrain(const ComPtr<IDirect3DDevice9>& graphicDevice):
	ViBuffer(graphicDevice)
{
}

auto ViBufferTerrain::GetPlane(const _float3& v_position) const -> D3DXPLANE
{
	const uint32_t index = static_cast<int32_t>(v_position.z / _interval) * _num_vertices_x + static_cast<int32_t>(v_position.x / _interval);

	const uint32_t	indices[4] = {
		index + _num_vertices_x,
		index + _num_vertices_x + 1,
		index + 1,
		index
	};

	const float	ratioX = (v_position.x - _vertices_pos[indices[0]].x) / _interval;
	const float	ratioZ = (_vertices_pos[indices[0]].z - v_position.z) / _interval;

	D3DXPLANE plane;

	/* ¿ì»ó´Ü */
	if (ratioX > ratioZ)
	{
		D3DXPlaneFromPoints(&plane, &_vertices_pos[indices[0]], &_vertices_pos[indices[1]], &_vertices_pos[indices[2]]);
	}

	/* ÁÂÇÏ´Ü */
	else
	{
		D3DXPlaneFromPoints(&plane, &_vertices_pos[indices[0]], &_vertices_pos[indices[2]], &_vertices_pos[indices[3]]);
	}

	return plane;
}

auto ViBufferTerrain::NativeConstructPrototype(const int32_t numVerticesX, const int32_t numVerticesZ,
                                               const float interval) -> HRESULT
{
#pragma region VERTICES
	_num_vertices = numVerticesX * numVerticesZ;
	_num_vertices_x = numVerticesX;
	_num_vertices_z = numVerticesZ;
	_interval = interval;
	_stride = sizeof(TagVertexNormalTexture);
	_vb_usage = 0;
	_fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	_primitive_type = D3DPT_TRIANGLELIST;
#pragma endregion

#pragma region INDICES
	_num_primitive = (numVerticesX - 1) * (numVerticesZ - 1) * 2;
	_num_vertices_per_primitive = 3;
	_ib_usage = 0;

	/* 2 or 4 */
	_one_index_size = 4;
#pragma endregion


	if (FAILED(ViBuffer::NativeConstructPrototype()))
		return E_FAIL;

	_vertices_pos = new _float3[_num_vertices];
	ZeroMemory(_vertices_pos, sizeof(_float3) * _num_vertices);

	TagVertexNormalTexture* vertices = nullptr;

	_vb->Lock(0, 0, reinterpret_cast<void**>(&vertices), 0);

	for (int32_t i = 0; i < numVerticesZ; ++i)
	{
		for (int32_t j = 0; j < numVerticesX; ++j)
		{
			const int32_t index = i * numVerticesX + j;
#pragma warning(push)
#pragma warning(disable: 6385)
#pragma warning(disable: 6386)
			vertices[index].v_position = _vertices_pos[index] = _float3(j * interval, 0.0f, i * interval);
			vertices[index].v_normal = _float3(0.0f, 1.f, 0.f);
			vertices[index].v_tex_uv = _float2(static_cast<float>(j) / (numVerticesX - 1) * 20.f, static_cast<float>(i) / (numVerticesZ - 1) * 20.f);
#pragma warning(pop)
		}
	}

	_vb->Unlock();


	int32_t* pIndices = 0;

	int32_t iVertexIndex = 0;

	_ib->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	for (int32_t i = 0; i < numVerticesZ - 1; ++i)
	{
		for (int32_t j = 0; j < numVerticesX - 1; ++j)
		{
			int32_t index = i * numVerticesX + j;

			// ¿ì»ó´Ü »ï°¢Çü. 
			pIndices[iVertexIndex++] = index + numVerticesX;
			pIndices[iVertexIndex++] = index + numVerticesX + 1;
			pIndices[iVertexIndex++] = index + 1;


			// ÁÂÇÏ´Ü »ï°¢Çü. 
			pIndices[iVertexIndex++] = index + numVerticesX;
			pIndices[iVertexIndex++] = index + 1;
			pIndices[iVertexIndex++] = index;
		}
	}

	_ib->Unlock();

	return S_OK;
}

auto ViBufferTerrain::NativeConstructPrototype(const std::wstring& heightMapPath, const float interval) -> HRESULT
{
	int32_t			numVerticesX = 0;
	int32_t			numVerticesZ = 0;

	HANDLE		hFile = CreateFile(heightMapPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;


	LPDWORD	dwByte = nullptr;

	BITMAPFILEHEADER		fh;
	bool check;
	check = ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), dwByte, nullptr);

	BITMAPINFOHEADER		ih;
	check = ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), dwByte, nullptr);

	numVerticesX = ih.biWidth;
	numVerticesZ = ih.biHeight;

	const int32_t arraySize = numVerticesX * numVerticesZ;
	const auto pixel = new uint64_t[arraySize];

	check = ReadFile(hFile, pixel, sizeof(int64_t) * numVerticesX * numVerticesZ, dwByte, nullptr);

	CloseHandle(hFile);

#pragma region VERTICES
	_num_vertices = numVerticesX * numVerticesZ;
	_num_vertices_x = numVerticesX;
	_num_vertices_z = numVerticesZ;
	_interval = interval;
	_stride = sizeof(TagVertexNormalTexture);
	_vb_usage = 0;
	_fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	_primitive_type = D3DPT_TRIANGLELIST;
#pragma endregion

#pragma region INDICES
	_num_primitive = (numVerticesX - 1) * (numVerticesZ - 1) * 2;
	_num_vertices_per_primitive = 3;
	_ib_usage = 0;

	/* 2 or 4 */
	_one_index_size = 4;
#pragma endregion


	if (FAILED(ViBuffer::NativeConstructPrototype()))
		return E_FAIL;

	TagVertexNormalTexture* vertices = nullptr;

	_vb->Lock(0, 0, reinterpret_cast<void**>(&vertices), 0);

	for (int32_t i = 0; i < numVerticesZ; ++i)
	{
		for (int32_t j = 0; j < numVerticesX; ++j)
		{
			const int32_t index = i * numVerticesX + j;
#pragma warning(push)
#pragma warning(disable: 6385)
#pragma warning(disable: 6386)
			_vertices_pos[index] = vertices[index].v_position = _float3(j * interval, (pixel[index] & 0x000000ff) / 10.0f, i * interval);
			vertices[index].v_normal = _float3(0.0f, 0.f, 0.f);
			D3DXVec3Normalize(&vertices[index].v_normal, &vertices[index].v_normal);
			vertices[index].v_tex_uv = _float2(static_cast<float>(j) / (numVerticesX - 1) * 20.f, static_cast<float>(i) / (numVerticesZ - 1) * 20.f);
#pragma warning(pop)
		}
	}

	int32_t* indices = nullptr;

	int32_t	vertexIndex = 0;

	_ib->Lock(0, 0, reinterpret_cast<void**>(&indices), 0);

	for (int32_t i = 0; i < numVerticesZ - 1; ++i)
	{
		for (int32_t j = 0; j < numVerticesX - 1; ++j)
		{
			const int32_t index = i * numVerticesX + j;

			// ¿ì»ó´Ü »ï°¢Çü. 
			indices[vertexIndex++] = static_cast<int32_t*>(_indices)[vertexIndex] = index + numVerticesX;
			indices[vertexIndex++] = static_cast<int32_t*>(_indices)[vertexIndex] = index + numVerticesX + 1;
			indices[vertexIndex++] = static_cast<int32_t*>(_indices)[vertexIndex] = index + 1;

			_float3	v_normal, vSour, vDest;

			vSour = vertices[index + 1].v_position - vertices[index + numVerticesX + 1].v_position;
			vDest = vertices[index + numVerticesX + 1].v_position - vertices[index + numVerticesX].v_position;

			D3DXVec3Cross(&v_normal, &vDest, &vSour);
			D3DXVec3Normalize(&v_normal, &v_normal);

			vertices[index + numVerticesX].v_normal += v_normal;
			vertices[index + numVerticesX + 1].v_normal += v_normal;
			vertices[index + 1].v_normal += v_normal;


			// ÁÂÇÏ´Ü »ï°¢Çü. 
			indices[vertexIndex++] = static_cast<int32_t*>(_indices)[vertexIndex] = index + numVerticesX;
			indices[vertexIndex++] = static_cast<int32_t*>(_indices)[vertexIndex] = index + 1;
			indices[vertexIndex++] = static_cast<int32_t*>(_indices)[vertexIndex] = index;

			vSour = vertices[index].v_position - vertices[index + 1].v_position;
			vDest = vertices[index + 1].v_position - vertices[index + numVerticesX].v_position;

			D3DXVec3Cross(&v_normal, &vDest, &vSour);
			D3DXVec3Normalize(&v_normal, &v_normal);

			vertices[index + numVerticesX].v_normal += v_normal;
			vertices[index + 1].v_normal += v_normal;
			vertices[index].v_normal += v_normal;

		}
	}

	for (int32_t i = 0; i < numVerticesX * numVerticesZ; ++i)
	{
		D3DXVec3Normalize(&vertices[i].v_normal, &vertices[i].v_normal);
	}


	_vb->Unlock();
	_ib->Unlock();
	delete[] pixel;
	return S_OK;
}

HRESULT ViBufferTerrain::NativeConstruct(void* arg)
{
	return S_OK;
}

auto ViBufferTerrain::Create(const ComPtr<IDirect3DDevice9>& graphicDevice, uint32_t numVerticesX,
                             const uint32_t numVerticesZ, const float interval) -> std::shared_ptr<ViBufferTerrain>
{
	std::shared_ptr<ViBufferTerrain> pInstance = std::make_shared<ViBufferTerrain>(graphicDevice);

	if (FAILED(pInstance->NativeConstructPrototype(numVerticesX, numVerticesZ, interval)))
	{
		MSGBOX("Failed to Creating CVIBuffer_Terrain");
		return nullptr;
	}

	return pInstance;
}

auto ViBufferTerrain::Create(const ComPtr<IDirect3DDevice9>& graphicDevice, const std::wstring& heightMapPath,
                             const float interval) -> std::shared_ptr<ViBufferTerrain>
{
	auto pInstance = std::make_shared<ViBufferTerrain>(graphicDevice);

	if (FAILED(pInstance->NativeConstructPrototype(heightMapPath, interval)))
	{
		MSGBOX("Failed to Creating CVIBuffer_Terrain");
		return nullptr;
	}
	return pInstance;
}

auto ViBufferTerrain::Clone(void* pArg) -> std::shared_ptr<Component>
{
	auto pInstance = std::make_shared<ViBufferTerrain>(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CVIBuffer_Terrain");
		return nullptr;
	}
	return pInstance;
}
