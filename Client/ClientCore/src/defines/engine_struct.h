#pragma once

typedef struct TagVertexTexture
{
	D3DXVECTOR3		v_position{};
	D3DXVECTOR2		v_tex_uv{};
} VtxTexture;

typedef struct TagVertexCubeTexture
{
	D3DXVECTOR3		v_position{};
	D3DXVECTOR3		v_tex_uv{};
} VtxCubeTexture;

typedef struct TagVertexNormalTexture
{
	D3DXVECTOR3		v_position{};
	D3DXVECTOR3		v_normal{};
	D3DXVECTOR2		v_tex_uv{};
} VtxNormalTexture;

