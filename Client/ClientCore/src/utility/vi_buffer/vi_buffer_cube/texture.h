#pragma once
#include "src/utility/components/component.h"

class Texture final : public Component
{
public:
	enum class kType
	{
		kTypeGeneral,
		kTypeCube,
		kTypeEnd
	};

public:
	explicit Texture(const ComPtr<IDirect3DDevice9>& graphicDevice);
	virtual ~Texture() = default;

public:
	virtual auto NativeConstructPrototype(kType eType, const std::wstring& textureFilePath, int32_t numTextures)->HRESULT;
	virtual auto NativeConstruct(void* pArg)->HRESULT;

public:
	auto SetUpOnGraphicDevice(int32_t stage, size_t textureIndex = 0)->HRESULT;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& graphicDevice, kType type, const std::wstring& textureFilePath, int32_t numTextures = 1)->std::shared_ptr<Texture>;
	virtual auto Clone(void* arg = nullptr)->std::shared_ptr<Component>;

private:
	std::vector<ComPtr<IDirect3DBaseTexture9>>			_textures;
};
