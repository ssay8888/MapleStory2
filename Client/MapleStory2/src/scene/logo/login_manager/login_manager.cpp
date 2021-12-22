#include "c_pch.h"
#include "login_manager.h"

#include "login_job_queue.h"

LoginManager::LoginManager() :
	_login_queue(std::make_shared<LoginJobQueue>())
{

}

void LoginManager::LoginResponse(const PacketSessionRef& session, const Protocol::LoginServerLogin& pkt)
{
	_login_queue->DoAsync(&LoginJobQueue::LoginAttempt, session, pkt);
}

void LoginManager::CharacterListResponse(const PacketSessionRef& session, const Protocol::LoginServerCharacterList& pkt)
{
	_login_queue->DoAsync(&LoginJobQueue::LoadCharacterList, session, pkt);
}

void LoginManager::CreateCharacterResponse(const PacketSessionRef& session, const Protocol::LoginServerCreateCharacter& pkt)
{
	_login_queue->DoAsync(&LoginJobQueue::CreateCharacter, session, pkt);
}

void LoginManager::CharacterSelectResponse(const PacketSessionRef& session,
                                           const Protocol::LoginServerCharacterSelect& pkt)
{
	_login_queue->DoAsync(&LoginJobQueue::CharacterSelect, session, pkt);
}
