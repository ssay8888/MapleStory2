#include "pch.h"
#include "auth_storage.h"

#include "login_session/login_session.h"
#include "randomizer/randomizer.h"

auto AuthStorage::CreateAuth(LoginSessionRef session) const -> std::shared_ptr<AuthInfo>
{
	const auto authInfo = MakeShared<AuthInfo>();
	authInfo->auth = Randomizer::Rand(0, INT64_MAX);
	authInfo->session = session;
	return authInfo;
}

auto AuthStorage::AddAuth(const std::shared_ptr<AuthInfo> auth)->void
{
	tbb::concurrent_hash_map<int64_t, std::shared_ptr<AuthInfo>>::accessor result;
	if (_auth_storage.insert(result, auth->session->GetAccountId()))
	{
		result->second = auth;
	}
}

auto AuthStorage::RemoveAuth(int64_t accountId) -> bool
{
	return _auth_storage.erase(accountId);
}

auto AuthStorage::FindAuth(int64_t accountId) const -> std::shared_ptr<AuthInfo>
{
	tbb::concurrent_hash_map<int64_t, std::shared_ptr<AuthInfo>>::const_accessor result;
	if (_auth_storage.find(result, accountId))
	{
		return result->second;
	}
	return nullptr;
}
