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
#ifndef __8d3668e7a9293f349502913684e4cb1c__
#define __8d3668e7a9293f349502913684e4cb1c__

#include "../util/git.h"
#include "../util/sqlite.h"
#include <memory>
#include <string>

class YipDirectory
{
public:
	YipDirectory(const std::string & projectPath);
	~YipDirectory();

	inline const std::string & path() const { return m_Path; }

	std::string writeFile(const std::string & path, const std::string & data, bool * changed = nullptr);
	std::string writeIncludeWrapper(const std::string & name, const std::string & originalIncludePath);

	std::string getGitRepositoryPath(const std::string & url);
	GitRepositoryPtr openGitRepository(const std::string & url, GitProgressPrinter * printer);
	GitRepositoryPtr openGitRepository(const std::string & url, GitProgressPrinter && prn = GitProgressPrinter())
		{ return openGitRepository(url, &prn); }

private:
	std::string m_Path;
	SQLiteDatabasePtr m_DB;

	void initDB();

	YipDirectory(const YipDirectory &) = delete;
	YipDirectory & operator=(const YipDirectory &) = delete;
};

typedef std::shared_ptr<YipDirectory> YipDirectoryPtr;

#endif
