#pragma once
#include <sql.h>
#include <sqlext.h>

enum
{
	kWvarcharMax = 4000,
	kBinaryMax = 8000
};

class DBConnection
{
public:
	auto Connect(SQLHENV henv, const WCHAR* connectionString) -> bool;
	auto Clear() -> void;

	auto Execute(const WCHAR* query) -> bool;
	auto Fetch() -> bool;
	auto GetRowCount() ->int32;
	auto Unbind() -> void;

public:
	auto BindParam(int32 paramIndex, bool* value, SQLLEN* index) -> bool;
	auto BindParam(int32 paramIndex, float* value, SQLLEN* index) -> bool;
	auto BindParam(int32 paramIndex, double* value, SQLLEN* index) -> bool;
	auto BindParam(int32 paramIndex, int8* value, SQLLEN* index) -> bool;
	auto BindParam(int32 paramIndex, int16* value, SQLLEN* index) -> bool;
	auto BindParam(int32 paramIndex, int32* value, SQLLEN* index) -> bool;
	auto BindParam(int32 paramIndex, int64* value, SQLLEN* index) -> bool;
	auto BindParam(int32 paramIndex, TIMESTAMP_STRUCT* value, SQLLEN* index) -> bool;
	auto BindParam(int32 paramIndex, const WCHAR* str, SQLLEN* index) -> bool;
	auto BindParam(int32 paramIndex, const BYTE* bin, int32 size, SQLLEN* index) -> bool;
		 
	auto BindCol(int32 columnIndex, bool* value, SQLLEN* index) -> bool;
	auto BindCol(int32 columnIndex, float* value, SQLLEN* index) -> bool;
	auto BindCol(int32 columnIndex, double* value, SQLLEN* index) -> bool;
	auto BindCol(int32 columnIndex, int8* value, SQLLEN* index) -> bool;
	auto BindCol(int32 columnIndex, int16* value, SQLLEN* index) -> bool;
	auto BindCol(int32 columnIndex, int32* value, SQLLEN* index) -> bool;
	auto BindCol(int32 columnIndex, int64* value, SQLLEN* index) -> bool;
	auto BindCol(int32 columnIndex, TIMESTAMP_STRUCT* value, SQLLEN* index) -> bool;
	auto BindCol(int32 columnIndex, WCHAR* str, int32 size, SQLLEN* index) -> bool;
	auto BindCol(int32 columnIndex, BYTE* bin, int32 size, SQLLEN* index) -> bool;

private:
	auto BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index) -> bool;
	auto BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index) -> bool;
	auto HandleError(SQLRETURN ret) -> void;

private:
	SQLHDBC			_connection = SQL_NULL_HANDLE;
	SQLHSTMT		_statement = SQL_NULL_HANDLE;
};

