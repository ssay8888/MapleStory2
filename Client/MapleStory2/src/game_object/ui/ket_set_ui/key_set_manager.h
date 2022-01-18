#pragma once
#include "src/utility/game_objects/game_object.h"

class KeySetUi;
class Shader;
class ViBufferRect;
class Texture;

class KeySetManager final : public GameObject
{
public:
	KeySetManager();
	~KeySetManager() override = default;

public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->HRESULT override;
	auto LateTick(double timeDelta)->HRESULT override;
	auto Render()->HRESULT override;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;

public:
	auto GetKeySet(int32_t index)->std::shared_ptr<KeySetUi>;
	auto IsCollisionKeySet()const->int32_t;

public:
	static auto Create()->std::shared_ptr<KeySetManager>;

private:
	auto AddComponents()->HRESULT;

private:
	std::shared_ptr<Shader>								_shader_com = nullptr;

	std::shared_ptr<KeySetUi>							_key_sets[16];

};

