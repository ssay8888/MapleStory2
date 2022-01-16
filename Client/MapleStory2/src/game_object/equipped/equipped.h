#pragma once
#include "src/utility/game_objects/game_object.h"

class Equipped final : public GameObject
{
public:
	Equipped();
	virtual ~Equipped() = default;

public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> int32_t override;
	auto LateTick(double timeDelta) -> int32_t override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;
	static auto Create()->std::shared_ptr<Equipped>;

public:
	auto FindItem(GameContents::kEquipeType type)->int32_t;
	auto AddItem(GameContents::kEquipeType type, int32_t itemId)->void;
	auto RemoveItem(GameContents::kEquipeType type)->void;
	auto GetAllItem()->std::vector<int32_t>;

private:
	std::map<GameContents::kEquipeType, int32_t>			_eqp_item;
};

