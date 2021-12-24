#include "game_server_pch.h"
#include "stats.h"

Stats::Stats(const int64_t characterId):
	_character_id(characterId)
{
}

auto Stats::GetCharacterId() const -> int64_t
{
	return _character_id;
}

auto Stats::Create(const int64_t characterId) -> std::shared_ptr<Stats>
{
	auto instance = MakeShared<Stats>(characterId);

	if (FAILED(instance->NativeConstruct()))
	{
		return nullptr;
	}

	return instance;
}

auto Stats::NativeConstruct() -> HRESULT
{
	return S_OK;
}
