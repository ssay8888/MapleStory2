#pragma once
#include "db_connection.h"

template<int32 C>
struct FullBits { enum { value = (1 << (C - 1)) | FullBits<C-1>::value }; };

template<>
struct FullBits<1> { enum { value = 1 }; };

template<>
struct FullBits<0> { enum { value = 0 }; };

template<int32 ParamCount, int32 ColumnCount>
class DBBind
{
public:
	DBBind(DBConnection& dbConnection, const WCHAR* query);

	auto Validate() -> bool;

	auto Execute() -> bool;

	auto Fetch() -> bool;

public:
	template<typename T>
	void BindParam(int32 idx, T& value)
	{
		_db_connection.BindParam(idx + 1, &value, &_param_index[idx]);
		_param_flag |= (1LL << idx);
	}

	void BindParam(int32 idx, const WCHAR* value)
	{
		_db_connection.BindParam(idx + 1, value, &_param_index[idx]);
		_param_flag |= (1LL << idx);
	}

	template<typename T, int32 N>
	void BindParam(int32 idx, T(&value)[N])
	{
		_db_connection.BindParam(idx + 1, (const BYTE*)value, size32(T) * N, &_param_index[idx]);
		_param_flag |= (1LL << idx);
	}

	template<typename T>
	void BindParam(int32 idx, T* value, int32 N)
	{
		_db_connection.BindParam(idx + 1, (const BYTE*)value, size32(T) * N, &_param_index[idx]);
		_param_flag |= (1LL << idx);
	}

	template<typename T>
	void BindCol(int32 idx, T& value)
	{
		_db_connection.BindCol(idx + 1, &value, &_column_index[idx]);
		_column_flag |= (1LL << idx);
	}

	template<int32 N>
	void BindCol(int32 idx, WCHAR(&value)[N])
	{
		_db_connection.BindCol(idx + 1, value, N - 1, &_column_index[idx]);
		_column_flag |= (1LL << idx);
	}

	void BindCol(int32 idx, WCHAR* value, int32 len)
	{
		_db_connection.BindCol(idx + 1, value, len - 1, &_column_index[idx]);
		_column_flag |= (1LL << idx);
	}

	template<typename T, int32 N>
	void BindCol(int32 idx, T(&value)[N])
	{
		_db_connection.BindCol(idx + 1, value, size32(T) * N, &_column_index[idx]);
		_column_flag |= (1LL << idx);
	}

protected:
	DBConnection&	_db_connection;
	const WCHAR*	_query;
	SQLLEN			_param_index[ParamCount > 0 ? ParamCount : 1];
	SQLLEN			_column_index[ColumnCount > 0 ? ColumnCount : 1];
	uint64			_param_flag;
	uint64			_column_flag;
};

template <int32 ParamCount, int32 ColumnCount>
DBBind<ParamCount, ColumnCount>::DBBind(DBConnection& dbConnection, const WCHAR* query): _db_connection(dbConnection), _query(query)
{
	::memset(_param_index, 0, sizeof(_param_index));
	::memset(_column_index, 0, sizeof(_column_index));
	_param_flag = 0;
	_column_flag = 0;
	dbConnection.Unbind();
}

template <int32 ParamCount, int32 ColumnCount>
auto DBBind<ParamCount, ColumnCount>::Validate() -> bool
{
	return _param_flag == FullBits<ParamCount>::value && _column_flag == FullBits<ColumnCount>::value;
}

template <int32 ParamCount, int32 ColumnCount>
auto DBBind<ParamCount, ColumnCount>::Execute() -> bool
{
	ASSERT_CRASH(Validate());
	return _db_connection.Execute(_query);
}

template <int32 ParamCount, int32 ColumnCount>
auto DBBind<ParamCount, ColumnCount>::Fetch() -> bool
{
	return _db_connection.Fetch();
}

