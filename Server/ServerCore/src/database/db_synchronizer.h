#pragma once
#include "db_connection.h"
#include "db_model.h"

/*--------------------
	DBSynchronizer
---------------------*/

class DBSynchronizer
{
	enum
	{
		kProcedureMaxLen = 10000
	};

	enum UpdateStep : uint8
	{
		kDropIndex,
		kAlterColumn,
		kAddColumn,
		kCreateTable,
		kDefaultConstraint,
		kCreateIndex,
		kDropColumn,
		kDropTable,
		kStoredProcecure,

		kMax
	};

	enum ColumnFlag : uint8
	{
		kType = 1 << 0,
		kNullable = 1 << 1,
		kIdentity = 1 << 2,
		kDefault = 1 << 3,
		kLength = 1 << 4,
	};

public:
	DBSynchronizer(DBConnection& conn) : _dbConn(conn) { }
	~DBSynchronizer();

	bool		Synchronize(const WCHAR* path);

private:
	void		ParseXmlDB(const WCHAR* path);
	bool		GatherDBTables();
	bool		GatherDBIndexes();
	bool		GatherDBStoredProcedures();

	void		CompareDBModel();
	void		CompareTables(DBModel::TableRef dbTable, DBModel::TableRef xmlTable);
	void		CompareColumns(DBModel::TableRef dbTable, DBModel::ColumnRef dbColumn, DBModel::ColumnRef xmlColumn);
	void		CompareStoredProcedures();

	void		ExecuteUpdateQueries();

private:
	DBConnection& _dbConn;

	Vector<DBModel::TableRef>			_xmlTables;
	Vector<DBModel::ProcedureRef>		_xmlProcedures;
	Set<String>							_xmlRemovedTables;

	Vector<DBModel::TableRef>			_dbTables;
	Vector<DBModel::ProcedureRef>		_dbProcedures;

private:
	Set<String>							_dependentIndexes;
	Vector<String>						_updateQueries[UpdateStep::kMax];
};