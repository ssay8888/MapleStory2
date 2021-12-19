#include "c_pch.h"
#include "characters_manager.h"

auto CharactersManager::PushCharacter(std::shared_ptr<Character> character) -> void
{
	_characters.push_back(character);
}

auto CharactersManager::GetCharacter(int32_t index) -> std::shared_ptr<Character>
{
	if (index >= _characters.size())
	{
		return nullptr;
	}
	return _characters[index];
}

auto CharactersManager::GetAllCharacter() const -> std::vector<std::shared_ptr<Character>>
{
	return _characters;
}
