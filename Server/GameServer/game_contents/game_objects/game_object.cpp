#include "pch.h"
#include "game_object.h"

GameObject::GameObject(): _state(kWait)
{
	D3DXMatrixIdentity(&_world_pos);
}

auto GameObject::SetMatrixState(const kMatrixState state, const D3DXVECTOR3 value) -> void
{
	*reinterpret_cast<D3DXVECTOR3*>(&_world_pos.m[state][0]) = value;
}

auto GameObject::SetState(kObjectState objectState) -> void
{
	_state = objectState;
}

auto GameObject::GetState() const -> kObjectState
{
	return _state;
}

auto GameObject::SetWorldPos(const D3DXMATRIX world) -> void
{
	_world_pos = world;
}

auto GameObject::GetWorldPos() const -> D3DXMATRIX
{
	return _world_pos;
}
