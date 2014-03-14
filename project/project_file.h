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
#ifndef __b452a1405c1a5aa54336e360b8b4fe1f__
#define __b452a1405c1a5aa54336e360b8b4fe1f__

#include "source_file.h"
#include "project_config.h"
#include "../util/git.h"
#include <vector>
#include <unordered_set>
#include <memory>

class ProjectFile : public std::enable_shared_from_this<ProjectFile>
{
public:
	ProjectFile(const std::string & prjPath);
	ProjectFile(const ProjectConfigPtr & prjConfig);
	~ProjectFile();

	inline bool isValid() const { return m_Valid; }
	inline void setValid(bool flag) { m_Valid = flag; }

	inline const std::string & projectPath() const { return m_ProjectPath; }
	const ProjectConfigPtr & config() const;

	SourceFilePtr addSourceFile(const std::string & name, const std::string & path);
	inline const std::vector<SourceFilePtr> & sourceFiles() const { return m_SourceFiles; }

	inline bool addRequirement(const std::string & req) { return m_Requires.insert(req).second; }
	inline const std::unordered_set<std::string> & requires() const { return m_Requires; }

	inline void addRepository(const GitRepositoryPtr & repo) { m_Repositories.push_back(repo); }
	inline const std::vector<GitRepositoryPtr> & repositories() const { return m_Repositories; }

private:
	std::string m_ProjectPath;
	std::vector<SourceFilePtr> m_SourceFiles;
	std::unordered_set<std::string> m_Requires;
	std::vector<GitRepositoryPtr> m_Repositories;
	mutable ProjectConfigPtr m_Config;
	bool m_Valid;

	ProjectFile(const ProjectFile &) = delete;
	ProjectFile & operator=(const ProjectFile &) = delete;
};

typedef std::shared_ptr<ProjectFile> ProjectFilePtr;

#endif
