#include "pch.h"
#include "shader.h"

#include "src/utility/components/textures/texture.h"

Shader::Shader(const ComPtr<IDirect3DDevice9>& graphicDevice):
	Component(graphicDevice)
{
}

auto Shader::NativeConstructPrototype(const std::wstring& shaderFilePath) -> HRESULT
{
	//ComPtr<ID3DXBuffer>	pErrorBuffer = nullptr;

	if (FAILED(D3DXCreateEffectFromFile(_graphic_device.Get(), shaderFilePath.c_str(), nullptr, nullptr, D3DXSHADER_DEBUG, nullptr, &_effect, nullptr/*pErrorBuffer*/)))
	{
		return E_FAIL;
		//MessageBoxA(0, (char*)pErrorBuffer->GetBufferPointer(), "Shader Build Error", MB_OK);		
	}

	return S_OK;
}

HRESULT Shader::NativeConstruct(void* arg)
{
	return S_OK;
}

auto Shader::SetUpConstantTable(const D3DXHANDLE hHandle, const void* data, const int32_t lengthByte) const -> HRESULT
{
	if (nullptr == _effect)
		return E_FAIL;

	if (FAILED(_effect->SetValue(hHandle, data, lengthByte)))
		return E_FAIL;

	return S_OK;
}

auto Shader::SetUpTextureConstantTable(D3DXHANDLE hHandle, const std::shared_ptr<Texture> textureCom, const int32_t textureIndex) -> HRESULT
{
	if (nullptr == _effect)
		return E_FAIL;

	const auto pTexture = textureCom->GetTexture(textureIndex);
	if (nullptr == pTexture)
		return E_FAIL;

	return _effect->SetTexture(hHandle, pTexture.Get());
}

auto Shader::SetUpTextureConstantTable(D3DXHANDLE hHandle, const ComPtr<IDirect3DBaseTexture9> texture) const -> HRESULT
{
	return _effect->SetTexture(hHandle, texture.Get());
}

auto Shader::SetUpBool(D3DXHANDLE hHandle, BOOL isBool) -> HRESULT
{
	if (nullptr == _effect)
		return E_FAIL;


	_effect->SetBool(hHandle, isBool);

	return S_OK;
}

auto Shader::BeginShader(const int32_t passIndex) const -> HRESULT
{
	if (nullptr == _effect)
		return E_FAIL;

	// m_pEffect->SetTechnique("DefaultTechnique");

	_effect->Begin(nullptr, 0);

	_effect->BeginPass(passIndex);

	return S_OK;
}

auto Shader::EndShader() const -> HRESULT
{
	if (nullptr == _effect)
		return E_FAIL;

	_effect->EndPass();
	_effect->End();
	return S_OK;
}

auto Shader::Commit() const -> void
{
	_effect->CommitChanges();
}

auto Shader::Create(const ComPtr<IDirect3DDevice9>& graphicDevice,
                    const std::wstring& shaderFilePath) -> std::shared_ptr<Shader>
{
	auto pInstance = std::make_shared<Shader>(graphicDevice);

	if (FAILED(pInstance->NativeConstructPrototype(shaderFilePath)))
	{
		MSGBOX("Failed to Creating CShader");
		return nullptr;
	}
	return pInstance;
}

auto Shader::Clone(void* arg) -> std::shared_ptr<Component>
{
	auto pInstance = std::make_shared<Shader>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone CShader");
		return nullptr;
	}
	return pInstance;
}
