#include "c_pch.h"
#include "character.h"

#include "src/game_object/equipped/equipped.h"

Character::Character()
{
}

Character::~Character()
{
}

auto Character::NativeConstruct() -> HRESULT
{
	_eqp_list = std::make_shared<Equipped>();
	return S_OK;
}

auto Character::Create() -> std::shared_ptr<Character>
{
	auto instance = std::make_shared<Character>();
	if (FAILED(instance->NativeConstruct()))
	{
		return nullptr;
	}
	return instance;
}

auto Character::SetInfo(const Protocol::ViewCharacter info)->void
{
	_info = info;
}

auto Character::GetInfo() const -> Protocol::ViewCharacter
{
	return _info;
}

auto Character::GetEqpList() const -> std::shared_ptr<Equipped>
{
	return _eqp_list;
}
