#pragma once
#include "data_reader/data_reader_manager.h"
#include "src/game_object/player/states/character_state.h"

class Monster;
class Collider;
class MapObject;

class LargeSwordAttackState final : public CharacterState
{
public:
	~LargeSwordAttackState() override = default;

	static auto GetInstance()->std::shared_ptr<LargeSwordAttackState>
	{
		static auto instance = std::make_shared<LargeSwordAttackState>();
		return instance;
	}

public:
	auto Enter() -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta) -> void override;
	auto LateTick(const double timeDelta) -> void override;
	auto Render() -> void override;

private:
	bool _input_combo = false;
	std::shared_ptr<DataReaderManager::Seq> _seq;
	std::shared_ptr<Collider> _aabb_com;

	std::map<int64_t, std::shared_ptr<Monster>> _monsters;

};

