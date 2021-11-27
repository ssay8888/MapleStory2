#include "pch.h"
#include "texture.h"

Texture::Texture(const ComPtr<IDirect3DDevice9>& graphicDevice):
	Component(graphicDevice)
{
}

auto Texture::GetTexture(const int32_t index) -> ComPtr<IDirect3DBaseTexture9>
{
	return _textures[index];
}

auto Texture::NativeConstructPrototype(kType eType, const std::wstring& textureFilePath,
                                       const int32_t numTextures) -> HRESULT
{
	_textures.reserve(numTextures);

	wchar_t	szTexturePath[MAX_PATH] = TEXT("");

	for (int32_t i = 0; i < numTextures; ++i)
	{
		ComPtr<IDirect3DBaseTexture9> pTexture = nullptr;
		//IDirect3DBaseTexture9* pTexture = nullptr;

		wsprintf(szTexturePath, textureFilePath.c_str(), i);

		//TODO : 이미지 출력이 안된다면 이곳을 확인하자.
		LPDIRECT3DTEXTURE9 tex = nullptr;
		if (eType == kType::kTypeGeneral)
		{
			if (FAILED(D3DXCreateTextureFromFile(_graphic_device.Get(), szTexturePath, &tex)))
			{
				return E_FAIL;
			}
		}
		else
		{
			if (FAILED(D3DXCreateCubeTextureFromFile(_graphic_device.Get(), szTexturePath, reinterpret_cast<LPDIRECT3DCUBETEXTURE9*>(tex))))
			{
				return E_FAIL;
			}
		}
		pTexture.Attach(tex);
		
		_textures.push_back(pTexture);
	}
	return S_OK;
}

HRESULT Texture::NativeConstruct(void* arg)
{
	return S_OK;
}

auto Texture::SetUpOnGraphicDevice(const int32_t stage, const size_t textureIndex) -> HRESULT
{
	if (nullptr == _graphic_device ||
		textureIndex >= _textures.size())
		return E_FAIL;

	return _graphic_device->SetTexture(stage, _textures[textureIndex].Get());
}

auto Texture::Create(const ComPtr<IDirect3DDevice9>& graphicDevice, const kType type, const std::wstring& textureFilePath,
                     const int32_t numTextures) -> std::shared_ptr<Texture>
{
	auto pInstance = std::make_shared<Texture>(graphicDevice);

	if (FAILED(pInstance->NativeConstructPrototype(type, textureFilePath, numTextures)))
	{
		MSGBOX("Failed to Creating Texture");
		return nullptr;
	}
	return pInstance;
}

auto Texture::Clone(void* arg) -> std::shared_ptr<Component>
{
	auto pInstance = std::make_shared<Texture>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone Texture");
		return nullptr;
	}
	return pInstance;
}
