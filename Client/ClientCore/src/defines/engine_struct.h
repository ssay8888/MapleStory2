#pragma once

typedef struct TagVertexTexture
{
	D3DXVECTOR3		position{};
	D3DXVECTOR2		tex_uv{};
} VtxTexture;

typedef struct TagVertexCubeTexture
{
	D3DXVECTOR3		position{};
	D3DXVECTOR3		tex_uv{};
} VtxCubeTexture;

typedef struct TagVertexNormalTexture
{
	D3DXVECTOR3		position{};
	D3DXVECTOR3		normal{};
	D3DXVECTOR2		tex_uv{};
} VtxNormalTexture;

typedef struct TagTextBoxInformation
{
	TagTextBoxInformation()
	{
		ZeroMemory(&font_info, sizeof(D3DXFONT_DESCW));
		font_info.Height = 11;
		font_info.Width = 11;
		font_info.Weight = FW_HEAVY;
		font_info.CharSet = HANGUL_CHARSET;
		lstrcpy(font_info.FaceName, L"µ¸¿òÃ¼");
	}
	D3DXFONT_DESCW		font_info{};
	D3DXVECTOR2 		pos{1.f, 1.f};
	D3DXVECTOR2			size{1.f, 1.f};
	int32_t				max_content_length{13};
	bool				is_password = false;
} TextBoxInformation;

typedef struct MeshMaterialTexture
{
	enum class kType { kTypeDiffuse, kTypeNormal, kTypeSpecular, kTypeEnd };

	Microsoft::WRL::ComPtr<IDirect3DTexture9>		diffuse_map;
	Microsoft::WRL::ComPtr<IDirect3DTexture9>		normal_map;
	Microsoft::WRL::ComPtr<IDirect3DTexture9>		specular_map;
	Microsoft::WRL::ComPtr<IDirect3DTexture9>		color_map;
}MeshMaterialTexture;

typedef struct D3DXMeshContainerDerived : public D3DXMESHCONTAINER
{
	MeshMaterialTexture** ppMaterialTextures = nullptr;

	Microsoft::WRL::ComPtr<ID3DXMesh> pOriginalMesh = nullptr;

	unsigned int			iNumBones = 0;
	D3DXMATRIX* pOffsetMatrices = nullptr;
	D3DXMATRIX* pRenderingMatrices = nullptr;
	D3DXMATRIX** ppCombindTransformationMatrices = nullptr;

}D3DxMeshContainerDerived;

typedef struct D3DxFrameDerived : public D3DXFRAME
{
	bool			is_cloned = false;
	D3DXMATRIX		CombinedTransformationMatrix;
}D3DxFrameDerived;

