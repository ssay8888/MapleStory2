#include "pch.h"
#include "db_model.h"
using namespace DBModel;
#include <regex>

/*-------------
	Column
--------------*/

String Column::CreateText()
{
	return Helpers::Format(
		L"[%s] %s %s %s",
		_name.c_str(),
		_typeText.c_str(),
		_nullable ? L"NULL" : L"NOT NULL",
		_identity ? Helpers::Format(L"IDENTITY(%d, %d)", _seedValue, _incrementValue).c_str() : L"");
}

/*-----------
	Index
------------*/

String Index::GetUniqueName()
{
	String ret;

	ret += _primaryKey ? L"PK " : L" ";
	ret += _uniqueConstraint ? L"UK " : L" ";
	ret += (_type == IndexType::Clustered ? L"C " : L"NC ");

	for (const ColumnRef& column : _columns)
	{
		ret += L"*";
		ret += column->_name;
		ret += L" ";
	}

	return ret;
}

String Index::CreateName(const String& tableName)
{
	String ret = L"IX_" + tableName;

	for (const ColumnRef& column : _columns)
	{
		ret += L"_";
		ret += column->_name;
	}

	return ret;
}

String Index::GetTypeText()
{
	return (_type == IndexType::Clustered ? L"CLUSTERED" : L"NONCLUSTERED");
}

String Index::GetKeyText()
{
	if (_primaryKey)
		return L"PRIMARY KEY";

	if (_uniqueConstraint)
		return L"UNIQUE";

	return L"";
}

String Index::CreateColumnsText()
{
	String ret;

	const int32_t size = static_cast<int32_t>(_columns.size());
	for (int32_t i = 0; i < size; i++)
	{
		if (i > 0)
			ret += L", ";

		ret += Helpers::Format(L"[%s]", _columns[i]->_name.c_str());
	}

	return ret;
}

bool Index::DependsOn(const String& columnName)
{
	auto findIt = std::find_if(_columns.begin(), _columns.end(),
		[&](const ColumnRef& column) { return column->_name == columnName; });

	return findIt != _columns.end();
}

/*-----------
	Table
------------*/

ColumnRef Table::FindColumn(const String& columnName)
{
	auto findIt = std::find_if(_columns.begin(), _columns.end(),
		[&](const ColumnRef& column) { return column->_name == columnName; });

	if (findIt != _columns.end())
		return *findIt;

	return nullptr;
}

/*----------------
	Procedures
-----------------*/

String Procedure::GenerateCreateQuery()
{
	const WCHAR* query = L"CREATE PROCEDURE [dbo].[%s] %s AS BEGIN %s END";

	String paramString = GenerateParamString();
	return Helpers::Format(query, _name.c_str(), paramString.c_str(), _body.c_str());
}

String Procedure::GenerateAlterQuery()
{
	const WCHAR* query = L"ALTER PROCEDURE [dbo].[%s] %s AS	BEGIN %s END";

	String paramString = GenerateParamString();
	return Helpers::Format(query, _name.c_str(), paramString.c_str(), _body.c_str());
}

String Procedure::GenerateParamString()
{
	String str;

	const int32_t size = static_cast<int32_t>(_parameters.size());
	for (int32_t i = 0; i < size; i++)
	{
		if (i < size - 1)
			str += Helpers::Format(L"\t%s %s,\n", _parameters[i]._name.c_str(), _parameters[i]._type.c_str());
		else
			str += Helpers::Format(L"\t%s %s", _parameters[i]._name.c_str(), _parameters[i]._type.c_str());
	}

	return str;
}


/*-------------
	Helpers
--------------*/

String Helpers::Format(const WCHAR* format, ...)
{
	WCHAR buf[4096];

	va_list ap;
	va_start(ap, format);
	vswprintf_s(buf, 4096, format, ap);
	va_end(ap);

	return String(buf);
}

String Helpers::DataType2String(DataType type)
{
	switch (type)
	{
	case DataType::kTinyInt:		return L"TinyInt";
	case DataType::kSmallInt:	return L"SmallInt";
	case DataType::kInt:			return L"Int";
	case DataType::kReal:		return L"Real";
	case DataType::kDateTime:	return L"DateTime";
	case DataType::kFloat:		return L"Float";
	case DataType::kBit:			return L"Bit";
	case DataType::kNumeric:		return L"Numeric";
	case DataType::kBigInt:		return L"BigInt";
	case DataType::kVarBinary:	return L"VarBinary";
	case DataType::kVarchar:		return L"Varchar";
	case DataType::kBinary:		return L"Binary";
	case DataType::kNVarChar:	return L"NVarChar";
	default:					return L"None";
	}
}

String Helpers::RemoveWhiteSpace(const String& str)
{
	String ret = str;

	ret.erase(
		std::remove_if(ret.begin(), ret.end(), [=](WCHAR ch) { return isspace(ch); }),
		ret.end());

	return ret;
}

DataType Helpers::String2DataType(const WCHAR* str, OUT int32_t& maxLen)
{
	std::wregex reg(L"([a-z]+)(\\((max|\\d+)\\))?");
	std::wcmatch ret;

	if (std::regex_match(str, OUT ret, reg) == false)
		return DataType::kNone;

	if (ret[3].matched)
		maxLen = _wcsicmp(ret[3].str().c_str(), L"max") == 0 ? -1 : _wtoi(ret[3].str().c_str());
	else
		maxLen = 0;

	if (_wcsicmp(ret[1].str().c_str(), L"TinyInt") == 0) return DataType::kTinyInt;
	if (_wcsicmp(ret[1].str().c_str(), L"SmallInt") == 0) return DataType::kSmallInt;
	if (_wcsicmp(ret[1].str().c_str(), L"Int") == 0) return DataType::kInt;
	if (_wcsicmp(ret[1].str().c_str(), L"Real") == 0) return DataType::kReal;
	if (_wcsicmp(ret[1].str().c_str(), L"DateTime") == 0) return DataType::kDateTime;
	if (_wcsicmp(ret[1].str().c_str(), L"Float") == 0) return DataType::kFloat;
	if (_wcsicmp(ret[1].str().c_str(), L"Bit") == 0) return DataType::kBit;
	if (_wcsicmp(ret[1].str().c_str(), L"Numeric") == 0) return DataType::kNumeric;
	if (_wcsicmp(ret[1].str().c_str(), L"BigInt") == 0) return DataType::kBigInt;
	if (_wcsicmp(ret[1].str().c_str(), L"VarBinary") == 0) return DataType::kVarBinary;
	if (_wcsicmp(ret[1].str().c_str(), L"Varchar") == 0) return DataType::kVarchar;
	if (_wcsicmp(ret[1].str().c_str(), L"Binary") == 0) return DataType::kBinary;
	if (_wcsicmp(ret[1].str().c_str(), L"NVarChar") == 0) return DataType::kNVarChar;

	return DataType::kNone;
}