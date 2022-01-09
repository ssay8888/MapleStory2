#pragma once
class Component;

class GameEntity abstract : public std::enable_shared_from_this<GameEntity>
{
public:
	GameEntity();
	virtual ~GameEntity() = default;

	virtual auto Tick(double timeDelta) ->void = 0;
	virtual auto LateTick(double timeDelta) ->void = 0;

public:
	auto GetObjectId()const->int64_t;

private:
	int64_t			_object_id;
};

