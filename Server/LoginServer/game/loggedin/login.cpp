#include "pch.h"
#include "login.h"

#include "login_session/login_session.h"
#include "src/database/db_connection_pool.h"
#include "src/database/db_bind.h"

Login::Login(LoginSessionRef session) :
	_session(session)
{
}

auto Login::Connected() -> void
{
	_state = kLoginState::kLoggedin;
}

auto Login::Disconnected() -> void
{
	const auto session = _session.lock();
	if (session)
	{
		if (auto con = DBConnectionPool::GetInstance().Pop())
		{
			DBBind<1, 0> bind(*con, L"{CALL dbo.spDisconnected(?)}");
			auto sessionId = session->GetAccountId();
			bind.BindParam(0, sessionId);

			if (bind.Execute())
			{
				_state = kLoginState::kNotLoggedin;
			}
			DBConnectionPool::GetInstance().Push(con);
		}
	}
}
