#pragma once
#include "types.h"

NAMESPACE_BEGIN(DBModel)

USING_SHARED_PTR(Column);
USING_SHARED_PTR(Index);
USING_SHARED_PTR(Table);
USING_SHARED_PTR(Procedure);

/*-------------
	DataType
--------------*/

enum class DataType
{
	kNone = 0,
	kTinyInt = 48,
	kSmallInt = 52,
	kInt = 56,
	kReal = 59,
	kDateTime = 61,
	kFloat = 62,
	kBit = 104,
	kNumeric = 108,
	kBigInt = 127,
	kVarBinary = 165,
	kVarchar = 167,
	kBinary = 173,
	kNVarChar = 231,
};

/*-------------
	Column
--------------*/

class Column
{
public:
	String				CreateText();

public:
	String				_name;
	int32_t				_columnId = 0; // DB
	DataType			_type = DataType::kNone;
	String				_typeText;
	int32_t				_maxLength = 0;
	bool				_nullable = false;
	bool				_identity = false;
	int64_t				_seedValue = 0;
	int64_t				_incrementValue = 0;
	String				_default;
	String				_defaultConstraintName; // DB
};

/*-----------
	Index
------------*/

enum class IndexType
{
	Clustered = 1,
	NonClustered = 2
};

class Index
{
public:
	String				GetUniqueName();
	String				CreateName(const String& tableName);
	String				GetTypeText();
	String				GetKeyText();
	String				CreateColumnsText();
	bool				DependsOn(const String& columnName);

public:
	String				_name; // DB
	int32_t				_indexId = 0; // DB
	IndexType			_type = IndexType::NonClustered;
	bool				_primaryKey = false;
	bool				_uniqueConstraint = false;
	Vector<ColumnRef>	_columns;
};

/*-----------
	Table
------------*/

class Table
{
public:
	ColumnRef			FindColumn(const String& columnName);

public:
	int32_t				_objectId = 0; // DB
	String				_name;
	Vector<ColumnRef>	_columns;
	Vector<IndexRef>	_indexes;
};

/*----------------
	Procedures
-----------------*/

struct Param
{
	String				_name;
	String				_type;
};

class Procedure
{
public:
	String				GenerateCreateQuery();
	String				GenerateAlterQuery();
	String				GenerateParamString();

public:
	String				_name;
	String				_fullBody; // DB
	String				_body; // XML
	Vector<Param>		_parameters;  // XML
};

/*-------------
	Helpers
--------------*/

class Helpers
{
public:
	static String		Format(const WCHAR* format, ...);
	static String		DataType2String(DataType type);
	static String		RemoveWhiteSpace(const String& str);
	static DataType		String2DataType(const WCHAR* str, OUT int32_t& maxLen);
};

NAMESPACE_END