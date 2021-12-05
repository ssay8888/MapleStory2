#pragma once
#include "src/utility/scene_utility/scene.h"

class CharacterSelect final : public Scene
{
public:
	explicit CharacterSelect(const ComPtr<IDirect3DDevice9>& device);
	~CharacterSelect() override = default;
public:
	auto NativeConstruct() -> HRESULT override;
	auto Tick(double timeDelta) -> int32_t override;
	auto Render() -> HRESULT override;
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<CharacterSelect>;

private:
	auto ReadyLight()->HRESULT;
	auto ReadyLayerCamera(const std::wstring& pLayerTag)->HRESULT;
	auto ReadyLayerPlayer(const std::wstring& pLayerTag)->HRESULT;
};

