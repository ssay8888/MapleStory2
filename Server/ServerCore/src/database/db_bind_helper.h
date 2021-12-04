#pragma once
#include "db_connection_pool.h"
#include "db_bind.h"
class DBConnection;

class DBBindHelper
{
public:
	static auto RegisterAccount(DBConnection& conn, const wchar_t* account, const wchar_t* password)->bool;

	template<int32_t N>
	static auto FindAccount(DBConnection& conn, const wchar_t* account, const wchar_t* password, wchar_t(&accountOut)[N], wchar_t(&passwordOut)[N])->bool
	{
		DBBind<1, 0> dbBind(conn, L"{CALL dbo.spGetAccount(?)}");
		dbBind.BindParam(0, account);
		auto result = dbBind.Execute();


		dbBind.BindCol(0, accountOut);
		dbBind.BindCol(1, passwordOut);
		dbBind.Fetch();

		return result;
	}
};