#pragma once
#include <d3dx9math.h>
class GameRoom;
class GameSession;
class Player : public std::enable_shared_from_this<Player>
{
public:
	Player(std::shared_ptr<GameSession> session);
	~Player() = default;

public:
	auto GetPos() const ->D3DXMATRIX;
	auto SetPos(D3DXMATRIX pos) -> void;
	auto GetRoom() const -> std::shared_ptr<GameRoom>;
	auto SetRoom(std::shared_ptr<GameRoom> room) -> void;
	auto GetScale() const -> float;
	auto SetScale(float scale) -> void;
	auto GainScale(float scale) -> void;

private:
	std::weak_ptr<GameSession> _session;
	std::weak_ptr<GameRoom> _current_room;
	float _scale;
	D3DXMATRIX _pos;
};