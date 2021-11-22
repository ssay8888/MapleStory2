#pragma once
class GameObject;

class GameRoom : public JobQueue
{
public:
	GameRoom();
	GameRoom(int32 max_user);
	~GameRoom();

	bool Enter(PacketSessionRef session);
	bool Leave(PacketSessionRef session);
	void DeadMonster(PacketSessionRef session, int64 object_id);
	void UserCollistion(PacketSessionRef session, int64 session_id);

	void Init();
	void InitMonster();
	void RespawnMonster();

public:
	void BroadCastSpawnPlayer(std::shared_ptr<class GameSession> session);
	void BroadCastMovePlayer(std::shared_ptr<class GameSession> session);
	void BroadCastRemovePlayer(std::shared_ptr<class GameSession> session);
	void BroadCastRespawnMonster(std::shared_ptr<class Monster> monster);
	void BroadCastRemoveMonster(std::shared_ptr<class Monster> monster);
	void BroadCastAllMonster(std::shared_ptr<class GameSession> session);
	void BroadCastChangeScale(std::shared_ptr<class GameSession> session);
	void BroadCastConnectedUserCount();
private:
	Map<int64, std::shared_ptr<class GameSession>>	_players;
	Vector<std::shared_ptr<GameObject>>				_objects;
	int32											_max_user;
};

