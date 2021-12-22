#include "game_server_pch.h"
#include "game_auth_manager.h"

#include "protocol/center_login_protocol.pb.h"

auto GameAuthManager::AddAuth(const Protocol::CenterLoginServerCreateAuth auth) -> void
{
	tbb::concurrent_hash_map<int64_t, std::shared_ptr<Protocol::CenterLoginServerCreateAuth>>::accessor result;
	if (_auth_storage.insert(result, auth.auth()))
	{
		auto a = MakeShared<Protocol::CenterLoginServerCreateAuth>(auth);
		result->second = a;
	}
}

auto GameAuthManager::FindAuth(int64_t auth) ->std::shared_ptr<Protocol::CenterLoginServerCreateAuth>
{
	tbb::concurrent_hash_map<int64_t, std::shared_ptr<Protocol::CenterLoginServerCreateAuth>>::const_accessor result;
	if (_auth_storage.find(result, auth))
	{
		return result->second;
	}
	return nullptr;
}
