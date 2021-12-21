#pragma once
#include "protocol/center_login_protocol.pb.h"

class GameAuthManager
{
public:
	static auto GetInstance()->GameAuthManager&
	{
		static GameAuthManager instance;
		return instance;
	}
	
	auto AddAuth(Protocol::CenterLoginServerCreateAuth auth)->void;

private:


private:
	tbb::concurrent_hash_map<int64_t, Protocol::CenterLoginServerCreateAuth> _auth_storage;
};

