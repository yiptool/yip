/* vim: set ai noet ts=4 sw=4 tw=115: */
//
// Copyright (c) 2014 Nikolay Zapolnov (zapolnov@gmail.com).
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#ifndef __e522f0443e271bf104c4f9cab88a6a39__
#define __e522f0443e271bf104c4f9cab88a6a39__

#include "../3rdparty/sqlite3/sqlite3.h"
#include <string>
#include <memory>
#include <initializer_list>
#include <ctime>
#include <functional>

class SQLiteTransaction;
class SQLiteCursor;

typedef std::function<void(const SQLiteCursor & cursor)> SQLiteCallback;

class SQLiteDatabase
{
public:
	SQLiteDatabase(const std::string & name);
	~SQLiteDatabase();

	inline const std::string & fileName() const { return m_DBFile; }
	inline sqlite3 * handle() const { return m_Handle; }

	inline void exec(const char * sql) { exec(sql, {}); }
	inline void exec(const std::string & sql) { exec(sql.c_str(), {}); }

	void exec(const char * sql, const std::initializer_list<std::string> & params);
	inline void exec(const std::string & sql, const std::initializer_list<std::string> & params)
		{ exec(sql.c_str(), params); }

	inline void select(const char * sql, SQLiteCallback onRow) { select(sql, {}, onRow); }
	inline void select(const std::string & sql, SQLiteCallback onRow) { select(sql.c_str(), {}, onRow); }

	void select(const char * sql, const std::initializer_list<std::string> & params, SQLiteCallback onRow);
	inline void select(const std::string & sql, const std::initializer_list<std::string> & params,
		SQLiteCallback onRow) { select(sql.c_str(), params, onRow); }

	int queryInt(const char * sql);
	inline int queryInt(const std::string & sql) { return queryInt(sql.c_str()); }

private:
	sqlite3 * m_Handle;
	sqlite3_stmt * m_StmtBegin;
	sqlite3_stmt * m_StmtRollback;
	sqlite3_stmt * m_StmtCommit;
	std::string m_DBFile;
	int m_InTransaction;
	bool m_TransactionFailed;

	void begin();
	void rollback();
	void commit();

	void prepare(sqlite3_stmt *& stmt, const char * sql);
	void bind(sqlite3_stmt * stmt, const std::initializer_list<std::string> & params);
	void exec(sqlite3_stmt * stmt, std::function<void()> = nullptr);

	SQLiteDatabase(const SQLiteDatabase &) = delete;
	SQLiteDatabase & operator=(const SQLiteDatabase &) = delete;

	friend class SQLiteTransaction;
};

typedef std::shared_ptr<SQLiteDatabase> SQLiteDatabasePtr;

class SQLiteTransaction
{
public:
	inline SQLiteTransaction(const SQLiteDatabasePtr & db) : m_DB(db), m_Committed(false) { m_DB->begin(); }
	inline ~SQLiteTransaction() { if (!m_Committed) m_DB->rollback(); }

	inline const SQLiteDatabasePtr & db() const { return m_DB; }

	void commit();

private:
	SQLiteDatabasePtr m_DB;
	bool m_Committed;

	SQLiteTransaction(const SQLiteTransaction &) = delete;
	SQLiteTransaction & operator=(const SQLiteTransaction &) = delete;
};

class SQLiteCursor
{
public:
	inline bool isNull(int index) const { return sqlite3_column_type(m_Cursor, index) == SQLITE_NULL; }
	inline int toInt(int index) const { return sqlite3_column_int(m_Cursor, index); }
	inline sqlite3_int64 toInt64(int index) const { return sqlite3_column_int64(m_Cursor, index); }
	inline size_t toSizeT(int index) const { return static_cast<size_t>(sqlite3_column_int64(m_Cursor, index)); }
	inline time_t toTimeT(int index) const { return static_cast<time_t>(sqlite3_column_int64(m_Cursor, index)); }
	inline double toDouble(int index) const { return sqlite3_column_double(m_Cursor, index); }

	inline const char * toText(int index) const
		{ return reinterpret_cast<const char *>(sqlite3_column_text(m_Cursor, index)); }

	inline std::string toString(int index) const
	{
		return std::string(
			reinterpret_cast<const char *>(sqlite3_column_blob(m_Cursor, index)),
			static_cast<size_t>(sqlite3_column_bytes(m_Cursor, index))
		);
	}

private:
	sqlite3_stmt * m_Cursor;

	inline SQLiteCursor(sqlite3_stmt * stmt) : m_Cursor(stmt) {}
	inline ~SQLiteCursor() {}

	SQLiteCursor(const SQLiteCursor &) = delete;
	SQLiteCursor & operator=(const SQLiteCursor &) = delete;

	friend class SQLiteDatabase;
};

#endif
