#pragma once
class Character;

class CharactersManager
{
public:
	static auto GetInstance()->CharactersManager&
	{
		static CharactersManager instance;
		return instance;
	}

public:
	auto PushCharacter(std::shared_ptr<Character> character)->void;
	auto GetCharacter(int32_t index)->std::shared_ptr<Character>;
	auto GetAllCharacter() const ->std::vector<std::shared_ptr<Character>>;

private:
	std::vector<std::shared_ptr<Character>> _characters;
};

