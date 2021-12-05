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
