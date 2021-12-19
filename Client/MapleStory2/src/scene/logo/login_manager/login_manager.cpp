#include "c_pch.h"
#include "login_manager.h"

#include "login_job_queue.h"

LoginManager::LoginManager() :
	_login_queue(std::make_shared<LoginJobQueue>())
{

}

void LoginManager::LoginResponse(PacketSessionRef& session, Protocol::ServerLogin& pkt)
{
	_login_queue->DoAsync(&LoginJobQueue::LoginAttempt, session, pkt);
}

void LoginManager::CharacterListResponse(PacketSessionRef& session, Protocol::ServerCharacterList& pkt)
{
	_login_queue->DoAsync(&LoginJobQueue::LoadCharacterList, session, pkt);
}

void LoginManager::CreateCharacterResponse(PacketSessionRef& session, Protocol::ServerCreateCharacter& pkt)
{
	_login_queue->DoAsync(&LoginJobQueue::CreateCharacter, session, pkt);
}
