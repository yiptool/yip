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
#include "../util/sha1.h"
#include "../util/fmt.h"
#include "../util/cxx_escape.h"

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

SourceFilePtr Project::addResourceFile(const std::string & name, const std::string & path)
{
	SourceFilePtr file = std::make_shared<SourceFile>(name, path);
	if (!m_ResourceFiles.insert(std::make_pair(name, file)).second)
		throw std::runtime_error(fmt() << "duplicate resource file '" << path << "'.");
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

void Project::addHeaderPath(const std::string & path, Platform::Type platform)
{
	auto it = m_HeaderPaths.find(path);
	if (it != m_HeaderPaths.end())
		it->second->setPlatforms(it->second->platforms() | platform);
	else
	{
		HeaderPathPtr headerPath = std::make_shared<HeaderPath>(path);
		m_HeaderPaths.insert(std::make_pair(path, headerPath));
	}
}

void Project::winrtAddLibrary(const std::string & name)
{
	m_WinRTLibraries.insert(name);
}

void Project::osxAddFramework(const std::string & name, const std::string & path)
{
	addFramework(m_OSXFrameworks, name, path, "OSX");
}

void Project::iosAddFramework(const std::string & name, const std::string & path)
{
	addFramework(m_IOSFrameworks, name, path, "iOS");
}

void Project::tizenAddPrivilege(const std::string & url)
{
	m_TizenPrivileges.insert(url);
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

void Project::generateLicenseData()
{
	std::stringstream ss;
	ss << "#include \".yip-import-proxies/yip/licenses.h\"\n";
	ss << "namespace YIP {\n";
	ss << "const char * const licenses[" << m_Licenses.size() << "] = {\n";
	for (const std::string & license : m_Licenses)
	{
		ss << "\t\"";
		cxxEscape(ss, license);
		ss << "\",\n";
	}
	ss << "};\n";
	ss << "}\n";
	std::string path = yipDirectory()->writeFile("licenses.cpp", ss.str());
	SourceFilePtr sourceFile = addSourceFile("yip/licenses.cpp", path);
	sourceFile->setIsGenerated(true);

	std::stringstream ss2;
	std::string guard = sha1(ss.str());
	ss2 << "#ifndef __" << guard << "__\n";
	ss2 << "#define __" << guard << "__\n";
	ss2 << "#include <cstddef>\n";
	ss2 << "namespace YIP {\n";
	ss2 << "extern const char * const licenses[" << m_Licenses.size() << "];\n";
	ss2 << "const size_t numLicenses = " << m_Licenses.size() << ";\n";
	ss2 << "}\n";
	ss2 << "#endif\n";
	path = yipDirectory()->writeFile(".yip-import-proxies/yip/licenses.h", ss2.str());
	sourceFile = addSourceFile("yip/licenses.h", path);
	sourceFile->setIsGenerated(true);
}
