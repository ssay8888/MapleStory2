#include "pch.h"
#include "game_room.h"

#include "game_contents/game_objects/monster/monster.h"
#include "game_session/game_session.h"
#include "game_session/game_session_manaeger.h"
#include "game_session/packet_creator.h"
#include "player/player.h"

GameRoom::GameRoom() : 
	_max_user(10)
{
	Init();
}

GameRoom::GameRoom(int32 max_user) :
	_max_user(max_user)
{
}

GameRoom::~GameRoom()
{
}

bool GameRoom::Enter(PacketSessionRef session)
{

	const GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);
	auto iter = _players.find(gameSession->GetSessionId());
	if (iter != _players.end())
	{
		return true;
	}
	gameSession->GetPlayer()->SetRoom(std::static_pointer_cast<GameRoom>(shared_from_this()));
	BroadCastSpawnPlayer(gameSession);
	BroadCastAllMonster(gameSession);

	_players.emplace(gameSession->GetSessionId(), gameSession);
	RespawnMonster();
	return false;
}

bool GameRoom::Leave(PacketSessionRef session)
{
	const GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);
	auto iter = _players.find(gameSession->GetSessionId());
	if (iter != _players.end())
	{
		_players.erase(gameSession->GetSessionId());
		BroadCastRemovePlayer(gameSession);
		return true;
	}

	RespawnMonster();
	return false;
}

void GameRoom::DeadMonster(PacketSessionRef session, int64 object_id)
{
	for (auto& obj : _objects)
	{
		auto monster = std::static_pointer_cast<Monster>(obj);
		if (monster->GetState() == GameObject::kAlive && monster->GetObjectId() == object_id)
		{
			const GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);
			D3DXVECTOR3 pos{ float(rand() % 30) , float(rand() % 30) , float(rand() % 30) };
			monster->SetMatrixState(GameObject::kStatePosition, pos);
			monster->SetState(GameObject::kDead);
			gameSession->GetPlayer()->GainScale(0.05);
			BroadCastChangeScale(gameSession);
			BroadCastRemoveMonster(monster);
			break;
		}
	}
	RespawnMonster();
}

void GameRoom::UserCollistion(PacketSessionRef session, int64 session_id)
{
	const GameSessionRef playerSession = std::static_pointer_cast<GameSession>(session);
	for (auto& player : _players)
	{
		const GameSessionRef userSession = std::static_pointer_cast<GameSession>(player.second);
		if (userSession->GetSessionId() == session_id)
		{
			if (userSession->GetPlayer()->GetScale() > 1.f && playerSession->GetPlayer()->GetScale() > userSession->GetPlayer()->GetScale())
			{
				playerSession->GetPlayer()->GainScale(0.1f);
				userSession->GetPlayer()->SetScale(1);
				BroadCastChangeScale(playerSession);
			}
			break;
		}
	}
	RespawnMonster();
}

void GameRoom::Init()
{
	InitMonster();
	RespawnMonster();
}

void GameRoom::InitMonster()
{
	const int32 monster_count = 30;
	_objects.reserve(monster_count);
	for (int i = 0; i < monster_count; ++i)
	{
		auto monster = MakeShared<Monster>(0.01f * (rand() % 500));
		monster->SetState(GameObject::kAlive);
		_objects.push_back(monster);
	}
	for (auto& obj : _objects)
	{
		D3DXVECTOR3 pos{ float(rand() % 30) , float(rand() % 30) , float(rand() % 30) };
		obj->SetMatrixState(GameObject::kStatePosition, pos);
	}
}

void GameRoom::RespawnMonster()
{
	for (auto& obj : _objects)
	{
		auto monster = std::static_pointer_cast<Monster>(obj);
		if (monster->GetState() == GameObject::kDead || monster->GetState() == GameObject::kWait)
		{
			D3DXVECTOR3 pos{ float(rand() % 30) , float(rand() % 30) , float(rand() % 30) };
			monster->SetMatrixState(GameObject::kStatePosition, pos);
			monster->SetState(GameObject::kAlive);
			BroadCastRespawnMonster(monster);
		}
	}
}

void GameRoom::BroadCastSpawnPlayer(std::shared_ptr<class GameSession> session)
{
	srand((unsigned int)time(NULL));
	
	PacketCreator::SpawnPlayer(session, session->GetSessionId());

	for (auto& player : _players)
	{
		//새로들어온 유저의 정보를 기존유저들에게 뿌려줌. 
		PacketCreator::AddUser(player.second, session);
		//이미 들어와있는 유저들의 정보를 접속한 유저에게 뿌려줌.
		PacketCreator::AddUser(session, player.second);
	}

}

void GameRoom::BroadCastMovePlayer(std::shared_ptr<GameSession> session)
{
	for (auto& player : _players)
	{
		// 기존 유저들에게 자신이 움직인정보를 보내줌.
		if (session != player.second)
		{
			PacketCreator::MovePlayer(session, player.second);
		}
	}
}

void GameRoom::BroadCastRemovePlayer(std::shared_ptr<GameSession> session)
{
	for (auto& player : _players)
	{
		// 기존 유저들에게 자신이 움직인정보를 보내줌.
		if (session != player.second)
		{
			PacketCreator::RemovePlayer(session, player.second);
		}
	}
}

void GameRoom::BroadCastRespawnMonster(std::shared_ptr<Monster> monster)
{
	for (auto& player : _players)
	{
		PacketCreator::RespawnMonster(player.second, monster);
	}
}

void GameRoom::BroadCastRemoveMonster(std::shared_ptr<Monster> monster)
{
	for (auto& player : _players)
	{
		PacketCreator::RemoveMonster(player.second, monster);
	}
}

void GameRoom::BroadCastAllMonster(std::shared_ptr<class GameSession> session)
{
	for (auto& obj : _objects)
	{
		auto monster = std::static_pointer_cast<Monster>(obj);
		if (monster!= nullptr && monster->GetState() == GameObject::kAlive)
		{
			PacketCreator::RespawnMonster(session, monster);
		}
	}
}

void GameRoom::BroadCastChangeScale(std::shared_ptr<GameSession> session)
{
	for (auto& player1 : _players)
	{
		for (auto& player2 : _players)
		{
			PacketCreator::ChangeScale(player1.second, player2.second, player1.second == player2.second);
		}
	}
}

void GameRoom::BroadCastConnectedUserCount()
{
	int count = _players.size();
	for (auto& player : _players)
	{
		PacketCreator::ConnectedUserCount(player.second, count);
	}
}
