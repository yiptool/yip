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
#include "define.h"
#include "yip_directory.h"
#include "../util/git.h"
#include <vector>
#include <map>
#include <memory>

class Project : public std::enable_shared_from_this<Project>
{
public:
	Project(const std::string & prjPath);
	~Project();

	inline bool isValid() const { return m_Valid; }
	inline void setValid(bool flag) { m_Valid = flag; }

	inline const std::string & projectPath() const { return m_ProjectPath; }
	const YipDirectoryPtr & yipDirectory() const;

	SourceFilePtr addSourceFile(const std::string & name, const std::string & path);
	inline const std::map<std::string, SourceFilePtr> & sourceFiles() const { return m_SourceFiles; }

	SourceFilePtr addResourceFile(const std::string & name, const std::string & path);
	inline const std::map<std::string, SourceFilePtr> & resourceFiles() const { return m_ResourceFiles; }

	DefinePtr addDefine(const std::string & name, Platform::Type platforms = Platform::All,
		BuildType::Value buildTypes = BuildType::All);
	inline const std::map<std::string, DefinePtr> & defines() const { return m_Defines; }

	inline bool addImport(const std::string & name, const std::string & url)
		{ return m_Imports.insert(std::make_pair(name, url)).second; }
	inline const std::map<std::string, std::string> & imports() const { return m_Imports; }

	// OSX

	void osxAddFramework(const std::string & name, const std::string & path);
	inline const std::map<std::string, std::string> & osxFrameworks() const { return m_OSXFrameworks; }

	// iOS

	void iosAddFramework(const std::string & name, const std::string & path);
	inline const std::map<std::string, std::string> & iosFrameworks() const { return m_IOSFrameworks; }

private:
	std::string m_ProjectPath;
	std::map<std::string, SourceFilePtr> m_SourceFiles;
	std::map<std::string, SourceFilePtr> m_ResourceFiles;
	std::map<std::string, DefinePtr> m_Defines;
	std::map<std::string, std::string> m_Imports;
	std::map<std::string, std::string> m_OSXFrameworks;
	std::map<std::string, std::string> m_IOSFrameworks;
	mutable YipDirectoryPtr m_YipDirectory;
	bool m_Valid;

	void addFramework(std::map<std::string, std::string> & map, const std::string & name,
		const std::string & path, const char * what);

	Project(const Project &) = delete;
	Project & operator=(const Project &) = delete;
};

typedef std::shared_ptr<Project> ProjectPtr;

#endif
