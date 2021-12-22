#pragma once
#include "src/utility/scene_utility/scene.h"

class GameInit final : public Scene
{
public:
	GameInit(const ComPtr<IDirect3DDevice9>& device);
	~GameInit() override = default;
public:
	auto NativeConstruct()->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;

	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<GameInit>;

private:
	bool _is_first_send = false;
};

