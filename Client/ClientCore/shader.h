#pragma once
#include "src/utility/components/component.h"

class Shader final : public Component
{
public:
	explicit Shader(const ComPtr<IDirect3DDevice9>& graphicDevice);
	virtual ~Shader() = default;

public:
	virtual auto NativeConstructPrototype(const std::wstring& shaderFilePath)->HRESULT;
	virtual auto NativeConstruct(void* arg)->HRESULT;
		
public:
	auto SetUpConstantTable(D3DXHANDLE hHandle, const void* data, int32_t lengthByte) const ->HRESULT;
	auto BeginShader(int32_t passIndex) const ->HRESULT;
	auto EndShader() const ->HRESULT;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& graphicDevice, const std::wstring& shaderFilePath)->std::shared_ptr<Shader>;
	auto Clone(void* arg)->std::shared_ptr<Component> override;

protected:
	ComPtr<ID3DXEffect>			_effect = nullptr;
};

