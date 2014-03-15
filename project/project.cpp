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
#include "project.h"
#include "../util/fmt.h"

Project::Project(const std::string & prjPath)
	: m_ProjectPath(prjPath),
	  m_Valid(true)
{
}

Project::~Project()
{
}

const YipDirectoryPtr & Project::yipDirectory() const
{
	if (!m_YipDirectory)
		m_YipDirectory = std::make_shared<YipDirectory>(m_ProjectPath);
	return m_YipDirectory;
}

SourceFilePtr Project::addSourceFile(const std::string & name, const std::string & path)
{
	SourceFilePtr file = std::make_shared<SourceFile>(name, path);
	if (!m_SourceFiles.insert(std::make_pair(name, file)).second)
		throw std::runtime_error(fmt() << "duplicate source file '" << path << "'.");
	return file;
}

DefinePtr Project::addDefine(const std::string & name, Platform::Type platforms, BuildType::Value buildTypes)
{
	DefinePtr define = std::make_shared<Define>(name);
	auto r = m_Defines.insert(std::make_pair(name, define));
	if (r.second)
	{
		define->setPlatforms(platforms);
		define->setBuildTypes(buildTypes);
	}
	else
	{
		define = r.first->second;
		define->setPlatforms(define->platforms() | platforms);
		define->setBuildTypes(define->buildTypes() | buildTypes);
	}
	return define;
}

void Project::osxAddFramework(const std::string & name, const std::string & path)
{
	addFramework(m_OSXFrameworks, name, path, "OSX");
}

void Project::iosAddFramework(const std::string & name, const std::string & path)
{
	addFramework(m_IOSFrameworks, name, path, "iOS");
}

void Project::addFramework(std::map<std::string, std::string> & map, const std::string & name,
	const std::string & path, const char * what)
{
	auto it = map.insert(std::make_pair(name, path));

	if (it.second)
		return;

	if (it.first->second != path)
	{
		if (path.empty())
			return;

		if (it.first->second.empty())
		{
			it.first->second = path;
			return;
		}

		throw std::runtime_error(fmt() << "conflicting declaration for " << what << " framework '" << name
			<< "' (was '" << it.first->second << "', now '" << path << "').");
	}
}
