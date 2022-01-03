#pragma once
#include "src/utility/scene_utility/scene.h"

class GamePlay final : public Scene
{
public:
	GamePlay(const ComPtr<IDirect3DDevice9>& device);
	virtual ~GamePlay() = default;

public:
	auto NativeConstruct()->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;

private:
	auto ReadyLight()->HRESULT;
	auto ReadyLayerCamera(const std::wstring& pLayerTag)->HRESULT;
	auto ReadyLayerPlayer(const std::wstring& pLayerTag)->HRESULT;
	auto ReadyLayerBackGround(const std::wstring& pLayerTag)->HRESULT;
	auto ReadyMonster()->HRESULT;
public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<GamePlay>;
};

