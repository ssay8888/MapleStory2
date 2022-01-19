#pragma once
#include "data_reader/data_reader_manager.h"
#include "src/game_object/player/states/character_state.h"

class Monster;
class Collider;

class BerserkerVolcanicslashState final : public CharacterState
{
public:
	~BerserkerVolcanicslashState() override = default;
	static auto GetInstance()->std::shared_ptr<BerserkerVolcanicslashState>
	{
		static auto instance = std::make_shared<BerserkerVolcanicslashState>();
		return instance;
	}

	auto Enter() -> void override;
	auto HandleInput() -> void override;
	auto Tick(const double timeDelta) -> void override;
	auto LateTick(const double timeDelta) -> void override;
	auto Render() -> void override;

private:
	bool _input_combo = false;
	bool _is_move;
	std::shared_ptr<DataReaderManager::Seq> _seq;
	std::shared_ptr<Collider> _aabb_com;
	std::map<std::wstring, float> _origin_moves;
	std::map<std::wstring, float> _moves;

	std::map<std::wstring, bool> _attacks;
	std::map<int64_t, std::shared_ptr<Monster>> _monsters;

};
