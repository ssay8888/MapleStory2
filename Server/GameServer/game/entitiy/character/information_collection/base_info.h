#pragma once
class BaseInfo abstract : std::enable_shared_from_this<BaseInfo>
{
public:
	explicit BaseInfo(int64_t characterId);
	~BaseInfo() = default;

public:
	auto GetCharacterId()const->int64_t;

private:
	int64_t _character_id;
};

