#pragma once
class Stats
{
public:
	explicit Stats(int64_t characterId);

public:
	auto GetCharacterId()const->int64_t;

public:
	static auto Create(const int64_t characterId)->std::shared_ptr<Stats>;

private:

	auto NativeConstruct()->HRESULT;
private:
	int64_t _character_id;
};

