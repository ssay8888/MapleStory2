#include "pch.h"
#include "player.h"

Player::Player(std::shared_ptr<GameSession> session) :
	_session(session),
	_scale(1.f)
{
	D3DXMatrixIdentity(&_pos);
}

auto Player::GetPos() const -> D3DXMATRIX
{
	return _pos;
}

auto Player::SetPos(D3DXMATRIX pos) -> void
{
	_pos = pos;
}

auto Player::GetRoom() const -> std::shared_ptr<GameRoom>
{
	return _current_room.lock();
}

auto Player::SetRoom(std::shared_ptr<GameRoom> room) -> void
{
	_current_room = room;
}

auto Player::GetScale() const -> float
{
	return _scale;
}

auto Player::SetScale(float scale) -> void
{
	_scale = scale;
}

auto Player::GainScale(float scale) -> void
{
	_scale += scale;
}
