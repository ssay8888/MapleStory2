#pragma once
#include "data_reader/data_reader_manager.h"
#include "src/utility/game_objects/game_object.h"

class MonsterStats;

class Monster final : public GameObject
{
public:
	explicit Monster(const ComPtr<IDirect3DDevice9>& device);
	virtual ~Monster() = default;
public:
	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> int32_t override;
	auto LateTick(double timeDelta) -> int32_t override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	static auto Create(const ComPtr<IDirect3DDevice9>& device)->std::shared_ptr<Monster>;

private:
	std::shared_ptr<DataReaderManager::MonsterInfo> _monster_info;
	std::shared_ptr<MonsterStats>					_monster_stat; 
};

