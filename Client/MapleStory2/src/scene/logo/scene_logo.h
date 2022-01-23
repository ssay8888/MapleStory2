#pragma once
#include "src/utility/scene_utility/scene.h"

class SceneLogo final : public Scene
{
public:
	SceneLogo(const ComPtr<IDirect3DDevice9>& device);
	~SceneLogo() override;
public:
	auto NativeConstruct() -> HRESULT override;
	auto Tick(double timeDelta) -> int32_t override;
	auto Render() -> HRESULT override;

	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<SceneLogo>;

private:

	auto ReadyLayerBackGround(const std::wstring& layerTag)->HRESULT;
	auto ReadyLayerTextBox(const std::wstring& layerTag)->HRESULT;
	auto ReadyMouse()->HRESULT;
};

