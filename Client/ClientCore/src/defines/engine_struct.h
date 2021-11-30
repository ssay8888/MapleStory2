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
		lstrcpy(font_info.FaceName, L"����ü");
	}
	D3DXFONT_DESCW		font_info{};
	D3DXVECTOR2 		pos{1.f, 1.f};
	D3DXVECTOR2			size{1.f, 1.f};
	int32_t				max_content_length{13};
} TextBoxInformation;




