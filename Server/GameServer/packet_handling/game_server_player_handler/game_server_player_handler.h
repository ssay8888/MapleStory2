#pragma once
namespace Protocol
{
	class GameClientAttackMonster;
}

class GameServerPlayerHandler
{
public:
	static auto TakeDamage(int64_t characterId, int64_t monsterObjectId, GameSessionRef gameSession)->void;

	static auto AttackMonster(Protocol::GameClientAttackMonster pkt, GameSessionRef gameSession)->void;
};

