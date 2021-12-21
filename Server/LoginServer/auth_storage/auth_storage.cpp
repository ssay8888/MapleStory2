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
