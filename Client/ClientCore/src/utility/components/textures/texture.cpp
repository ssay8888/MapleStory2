#include "pch.h"
#include "texture.h"

#include <iostream>

Texture::Texture(const ComPtr<IDirect3DDevice9>& graphicDevice):
	Component(graphicDevice)
{
}

auto Texture::GetTexture(const int32_t index) -> ComPtr<IDirect3DBaseTexture9>
{
	return _textures[index].first;
}

auto Texture::GetImageInfo(const int32_t index) const -> D3DXIMAGE_INFO
{
	return _textures[index].second;
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
		IDirect3DBaseTexture9* tex = nullptr;
		D3DXIMAGE_INFO imginfo;
		if (eType == kType::kTypeGeneral)
		{

			D3DXGetImageInfoFromFile(szTexturePath, &imginfo);
			/*if (FAILED(D3DXCreateTextureFromFile(_graphic_device.Get(), szTexturePath, reinterpret_cast<LPDIRECT3DTEXTURE9*>(&tex))))
			{
				return E_FAIL;
			}*/
			if (FAILED(D3DXCreateTextureFromFileExW(_graphic_device.Get(), szTexturePath, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0,
				D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
				D3DX_DEFAULT,
				0, NULL, NULL, reinterpret_cast<LPDIRECT3DTEXTURE9*>(&tex))))
			{
				return E_FAIL;
			}
		}
		else
		{
			D3DXGetImageInfoFromFile(szTexturePath, &imginfo);
			if (FAILED(D3DXCreateCubeTextureFromFile(_graphic_device.Get(), szTexturePath, reinterpret_cast<LPDIRECT3DCUBETEXTURE9*>(&tex))))
			{
				return E_FAIL;
			}
		}
		pTexture.Attach(tex);
		
		_textures.push_back(std::make_pair(pTexture, imginfo));
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

	return _graphic_device->SetTexture(stage, _textures[textureIndex].first.Get());
}

auto Texture::Create(const ComPtr<IDirect3DDevice9>& graphicDevice, const kType type, const std::wstring& textureFilePath,
                     const int32_t numTextures) -> std::shared_ptr<Texture>
{
	auto pInstance = std::make_shared<Texture>(graphicDevice);

	if (FAILED(pInstance->NativeConstructPrototype(type, textureFilePath, numTextures)))
	{
		std::wcout << L"Failed to Create Texture " << textureFilePath << std::endl;
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
