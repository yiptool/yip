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
#include "project_config.h"
#include "../util/fmt.h"
#include "../util/path.h"
#include "../util/sha1.h"
#include <iostream>
#include <cerrno>
#include <cstring>

#define DATABASE_VERSION 1

ProjectConfig::ProjectConfig(const std::string & prjPath)
	: m_Path(pathConcat(prjPath, ".yip")),
	  m_ProjectPath(prjPath)
{
	pathCreate(m_Path);

	m_DB = std::make_shared<SQLiteDatabase>(pathConcat(m_Path, "db"));
	initDB();
}

ProjectConfig::~ProjectConfig()
{
}

void ProjectConfig::writeFile(const std::string & path, const std::string & data)
{
	std::string file = pathSimplify(pathConcat(m_Path, path));
	bool has_sha1 = false, write = true;
	std::string new_sha1;

	SQLiteTransaction transaction(m_DB);

	// Check whether file has changed
	if (pathIsExistent(file))
	{
		// Canonicalize file path
		file = pathMakeCanonical(file);

		// Get information about file from the database
		bool found = false;
		size_t old_size = 0;
		time_t old_time = 0;
		std::string old_sha1;
		m_DB->select("SELECT size, time, sha1 FROM files WHERE path = ? LIMIT 1", { file },
			[&found, &old_size, &old_time, &old_sha1](const SQLiteCursor & cursor) {
				found = true;
				old_size = cursor.toSizeT(0);
				old_time = cursor.toTimeT(1);
				old_sha1 = cursor.toString(2);
			}
		);

		// Check whether file has been modified
		if (found && data.size() == old_size)
		{
			if (pathGetModificationTime(file) <= old_time)
			{
				new_sha1 = sha1(data);
				has_sha1 = true;
				if (new_sha1 == old_sha1)
					write = false;
			}
		}
	}

	// Do not overwrite file if it did not change
	if (!write)
	{
		std::cout << "Keeping " << path << std::endl;
		return;
	}

	std::cout << "Writing " << path << std::endl;

	// Calculate SHA1 sum of the file
	if (!has_sha1)
		new_sha1 = sha1(data);

	// Create directory for the file
	std::string dir = pathGetDirectory(file);
	if (dir.length() > 0)
		pathCreate(dir);

	// Write the file
	FILE * f = fopen(file.c_str(), "wb");
	if (!f)
	{
		int err = errno;
		throw std::runtime_error(fmt() << "unable to create file '" << file << "': " << strerror(err));
	}
	try
	{
		fwrite(data.data(), 1, data.size(), f);
		if (ferror(f))
		{
			int err = errno;
			throw std::runtime_error(fmt() << "unable to write file '" << file << "': " << strerror(err));
		}

		fflush(f);
		if (ferror(f))
		{
			int err = errno;
			throw std::runtime_error(fmt() << "unable to write file '" << file << "': " << strerror(err));
		}
	}
	catch (...)
	{
		fclose(f);
		remove(file.c_str());
		throw;
	}
	fclose(f);

	// Store information about file into the database
	m_DB->exec(fmt() << "REPLACE INTO files (path, size, time, sha1) VALUES (?, " << data.size() << ", "
		<< time(nullptr) << ", ?)", { file, new_sha1 });
	transaction.commit();
}

GitRepositoryPtr ProjectConfig::openGitRepository(const std::string & url, GitProgressPrinter && printer)
{
	std::string shortDirName = "git-" + sha1(url).substr(1, 10);
	std::string dirName = pathConcat(m_Path, shortDirName);

	// Open the repository
	GitRepositoryPtr repo;
	try {
		repo = GitRepository::openEx(dirName, GIT_REPOSITORY_OPEN_NO_SEARCH);
	} catch (const GitError &) {
		repo = GitRepository::clone(dirName, url, &printer);
	}

	return repo;
}

void ProjectConfig::initDB()
{
	// Create tables
	m_DB->exec("CREATE TABLE IF NOT EXISTS version (id INTEGER PRIMARY KEY, value INTEGER);");
	m_DB->exec("CREATE TABLE IF NOT EXISTS files (path TEXT PRIMARY KEY, size INTEGER, "
		"time INTEGER, sha1 TEXT);");

	SQLiteTransaction transaction(m_DB);

	// Check database version
	int version = m_DB->queryInt("SELECT value FROM version WHERE id = 1 LIMIT 1");
	if (version > DATABASE_VERSION)
	{
		throw std::runtime_error(fmt() << "database version " << version
			<< " is not supported (maximum supported version is " << DATABASE_VERSION << ").");
	}

	// Update database version
	m_DB->exec(fmt() << "REPLACE INTO version (id, value) VALUES (1, " << DATABASE_VERSION << ")");

	transaction.commit();

	if (version != 0 && version != DATABASE_VERSION)
		std::cout << "notice: database has been updated to version " << DATABASE_VERSION << '.' << std::endl;
}
