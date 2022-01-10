#pragma once
#include "data_reader/data_reader_manager.h"
#include "src/game_object/monster/states/monster_state.h"

class MonsterAttackAState : public MonsterState
{
public:
	explicit MonsterAttackAState(Protocol::kMonsterState state);
	~MonsterAttackAState() override = default;
	auto Enter(std::shared_ptr<Monster> monster) -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta, std::shared_ptr<Monster> monster) -> void override;
	auto LateTick(const double timeDelta, std::shared_ptr<Monster> monster) -> void override;
	auto Render(std::shared_ptr<Monster> monster) -> void override;


private:
	const Protocol::kMonsterState _state;
	std::vector<std::shared_ptr<DataReaderManager::Motion>> _use_motions;
	std::shared_ptr<DataReaderManager::Motion> _use_skill;
	std::vector<std::pair<int32_t, int32_t>> _skills;
};

