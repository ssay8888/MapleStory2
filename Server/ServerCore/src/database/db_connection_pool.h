#pragma once
#include "db_connection.h"

class DBConnectionPool
{
private:
	DBConnectionPool();
	~DBConnectionPool();

public:
	static auto GetInstance() -> DBConnectionPool&
	{
		static DBConnectionPool instance;
		return instance;
	}

	auto Connect(int32 connectionCount, const WCHAR* connectionString) -> bool;
	auto Clear() -> void;

	auto Pop() -> DBConnection*;
	auto Push(DBConnection* connection) -> void;

private:
	USE_LOCK;
	SQLHENV					_environment = SQL_NULL_HANDLE;
	Vector<DBConnection*>	_connections;
};

