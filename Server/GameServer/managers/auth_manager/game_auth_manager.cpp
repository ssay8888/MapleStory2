#include "game_server_pch.h"
#include "game_auth_manager.h"

#include "protocol/center_login_protocol.pb.h"

auto GameAuthManager::AddAuth(const Protocol::CenterLoginServerCreateAuth auth) -> void
{
	tbb::concurrent_hash_map<int64_t, Protocol::CenterLoginServerCreateAuth>::accessor result;
	if (_auth_storage.insert(result, auth.accountid()))
	{
		result->second = auth;
	}
}
