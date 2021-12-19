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
	auto GetRowCount() ->int32_t;
	auto Unbind() -> void;

public:
	auto BindParam(int32_t paramIndex, bool* value, SQLLEN* index) -> bool;
	auto BindParam(int32_t paramIndex, float* value, SQLLEN* index) -> bool;
	auto BindParam(int32_t paramIndex, double* value, SQLLEN* index) -> bool;
	auto BindParam(int32_t paramIndex, int8_t* value, SQLLEN* index) -> bool;
	auto BindParam(int32_t paramIndex, int16_t* value, SQLLEN* index) -> bool;
	auto BindParam(int32_t paramIndex, int32_t* value, SQLLEN* index) -> bool;
	auto BindParam(int32_t paramIndex, int64_t* value, SQLLEN* index) -> bool;
	auto BindParam(int32_t paramIndex, TIMESTAMP_STRUCT* value, SQLLEN* index) -> bool;
	auto BindParam(int32_t paramIndex, const WCHAR* str, SQLLEN* index) -> bool;
	auto BindParam(int32_t paramIndex, const BYTE* bin, int32_t size, SQLLEN* index) -> bool;
	auto BindParamOutput(int32_t paramIndex, int32_t* value, SQLLEN* index) -> bool;
	auto BindParamOutput(int32_t paramIndex, int64_t* value, SQLLEN* index) -> bool;
		 
	auto BindCol(int32_t columnIndex, bool* value, SQLLEN* index) -> bool;
	auto BindCol(int32_t columnIndex, float* value, SQLLEN* index) -> bool;
	auto BindCol(int32_t columnIndex, double* value, SQLLEN* index) -> bool;
	auto BindCol(int32_t columnIndex, int8_t* value, SQLLEN* index) -> bool;
	auto BindCol(int32_t columnIndex, int16_t* value, SQLLEN* index) -> bool;
	auto BindCol(int32_t columnIndex, int32_t* value, SQLLEN* index) -> bool;
	auto BindCol(int32_t columnIndex, int64_t* value, SQLLEN* index) -> bool;
	auto BindCol(int32_t columnIndex, TIMESTAMP_STRUCT* value, SQLLEN* index) -> bool;
	auto BindCol(int32_t columnIndex, WCHAR* str, int32_t size, SQLLEN* index) -> bool;
	auto BindCol(int32_t columnIndex, BYTE* bin, int32_t size, SQLLEN* index) -> bool;
	
private:
	auto BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index) -> bool;
	auto BindParamOut(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index) -> bool;
	auto BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index) -> bool;
	auto HandleError(SQLRETURN ret) -> void;

private:
	SQLHDBC			_connection = SQL_NULL_HANDLE;
	SQLHSTMT		_statement = SQL_NULL_HANDLE;
};


