#include "pch.h"
#include "db_bind_helper.h"

#include "db_bind.h"

auto DBBindHelper::RegisterAccount(DBConnection& conn, const wchar_t* account, const wchar_t* password) -> bool
{
	DBBind<2, 0> dbBind(conn, L"{CALL dbo.spInsertAccount(?,?)}");
	dbBind.BindParam(0, account);
	dbBind.BindParam(1, password);
	const auto result = dbBind.Execute();
	return result;
}
