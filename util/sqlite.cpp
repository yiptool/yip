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
#include "sqlite.h"
#include "fmt.h"
#include <stdexcept>
#include <iostream>

/* SQLiteDatabase */

SQLiteDatabase::SQLiteDatabase(const std::string & name)
	: m_StmtBegin(nullptr),
	  m_StmtRollback(nullptr),
	  m_StmtCommit(nullptr),
	  m_DBFile(name),
	  m_InTransaction(0),
	  m_TransactionFailed(false)
{
	int err = sqlite3_open_v2(name.c_str(), &m_Handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
	if (err != SQLITE_OK)
	{
		throw std::runtime_error(fmt()
			<< "unable to open sqlite database '" << name << "': " << sqlite3_errstr(err));
	}
}

SQLiteDatabase::~SQLiteDatabase()
{
	if (m_StmtBegin)
		sqlite3_finalize(m_StmtBegin);
	if (m_StmtRollback)
		sqlite3_finalize(m_StmtRollback);
	if (m_StmtCommit)
		sqlite3_finalize(m_StmtCommit);

	int err = sqlite3_close(m_Handle);
	if (err != SQLITE_OK)
	{
		std::cerr << "warning: unable to close sqlite database '" << m_DBFile << "': "
			<< sqlite3_errstr(err) << std::endl;
		sqlite3_close_v2(m_Handle);
	}
}

void SQLiteDatabase::exec(const char * sql)
{
	sqlite3_stmt * stmt = nullptr;
	prepare(stmt, sql);

	try
	{
		exec(stmt);
	}
	catch (...)
	{
		sqlite3_finalize(stmt);
		throw;
	}

	sqlite3_finalize(stmt);
}

int SQLiteDatabase::queryInt(const char * sql)
{
	int result = 0;

	sqlite3_stmt * stmt = nullptr;
	prepare(stmt, sql);

	try
	{
		exec(stmt, [stmt, &result]() {
			result = sqlite3_column_int(stmt, 0);
		});
	}
	catch (...)
	{
		sqlite3_finalize(stmt);
		throw;
	}

	sqlite3_finalize(stmt);

	return result;
}

void SQLiteDatabase::begin()
{
	if (m_InTransaction)
		return;

	m_TransactionFailed = false;
	prepare(m_StmtBegin, "BEGIN IMMEDIATE");
	prepare(m_StmtRollback, "ROLLBACK");
	exec(m_StmtBegin);
	++m_InTransaction;
}

void SQLiteDatabase::rollback()
{
	if (!m_InTransaction)
		throw std::runtime_error("attempted to invoke 'rollback' outside of transaction.");

	m_TransactionFailed = true;
	if (--m_InTransaction == 0)
		exec(m_StmtRollback);
}

void SQLiteDatabase::commit()
{
	if (!m_InTransaction)
		throw std::runtime_error("attempted to invoke 'commit' outside of transaction.");

	if (--m_InTransaction > 0)
		return;

	if (m_TransactionFailed)
	{
		exec(m_StmtRollback);
		return;
	}

	try
	{
		prepare(m_StmtCommit, "COMMIT");
		exec(m_StmtCommit);
	}
	catch (const std::exception & e)
	{
		std::cerr << "error: database commit failed: " << e.what() << std::endl;
		exec(m_StmtRollback);
		throw;
	}
}

void SQLiteDatabase::prepare(sqlite3_stmt *& stmt, const char * sql)
{
	if (stmt)
		return;

	int err = sqlite3_prepare_v2(m_Handle, sql, -1, &stmt, nullptr);
	if (err != SQLITE_OK || !stmt)
	{
		throw std::runtime_error(fmt()
			<< "unable to prepare statement '" << sql << "': " << sqlite3_errmsg(m_Handle));
	}
}

void SQLiteDatabase::exec(sqlite3_stmt * stmt, std::function<void()> onRow)
{
	for (;;)
	{
		int err = sqlite3_step(stmt);
		if (err == SQLITE_DONE)
		{
			sqlite3_reset(stmt);
			return;
		}
		else if (err != SQLITE_ROW)
		{
			throw std::runtime_error(fmt()
				<< "unable to execute statement '" << sqlite3_sql(stmt) << "': " << sqlite3_errmsg(m_Handle));
		}

		try
		{
			onRow();
		}
		catch (...)
		{
			sqlite3_reset(stmt);
			throw;
		}
	}
}


/* SQLiteTransaction */

void SQLiteTransaction::commit()
{
	if (!m_Committed)
	{
		m_DB->commit();
		m_Committed = true;
	}
}
