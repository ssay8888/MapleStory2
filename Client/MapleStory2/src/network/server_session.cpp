#include "c_pch.h"
#include "server_session.h"

#include "src/main/main_app.h"

ServerSession::ServerSession()
{
}

void ServerSession::OnConnected()
{
	if (g_mainApp)
	{
		g_mainApp->SetConnected(true);
	}
}

void ServerSession::OnRecvPacket(BYTE* buffer,  int32_t len)
{
}

void ServerSession::OnSend(int32_t len)
{
}

void ServerSession::OnDisconnected()
{
}
