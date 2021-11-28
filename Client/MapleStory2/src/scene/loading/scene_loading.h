#pragma once
#include "src/utility/scene_utility/scene.h"

class SceneLoading final : public Scene
{
public:
	explicit SceneLoading(const ComPtr<IDirect3DDevice9>& device);
	virtual ~SceneLoading() = default;

public:
	auto NativeConstruct(kScene nextScene)->HRESULT ;
	auto Tick(const double timeDelta)->int32_t override;
	auto Render()->HRESULT override;

public:
	static std::shared_ptr<SceneLoading> Create(const ComPtr<IDirect3DDevice9>& device, kScene nextLevel);

private:
	auto ReadyLayerBackGround(const std::wstring& layerTag)->HRESULT;
	auto ReadyLayerUserInterface(const std::wstring& layerTag)->HRESULT;

private:
	std::shared_ptr<class Loading> _loader = nullptr;
	kScene _next_scene = kScene::kSceneEnd;
};

