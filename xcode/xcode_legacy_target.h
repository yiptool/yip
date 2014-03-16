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
#ifndef __fe4fe52416376ff28413526b1a5464ee__
#define __fe4fe52416376ff28413526b1a5464ee__

#include "xcode_object.h"
#include <vector>

class XCodeProject;
class XCodeConfigurationList;
class XCodeBuildPhase;
class XCodeTargetDependency;

class XCodeLegacyTarget : public XCodeObject
{
public:
	inline std::string objectName() const { return m_Name; }

	inline const std::string & name() const { return m_Name; }
	inline void setName(const std::string & name_) { m_Name = name_; }

	inline const std::string & productName() const { return m_ProductName; }
	inline void setProductName(const std::string & name_) { m_ProductName = name_; }

	inline XCodeConfigurationList * buildConfigurationList() const { return m_BuildConfigurationList; }
	inline void setBuildConfigurationList(XCodeConfigurationList * cfg) { m_BuildConfigurationList = cfg; }

	inline const std::string & buildArgumentsString() const { return m_BuildArgumentsString; }
	inline void setBuildArgumentsString(const std::string & str) { m_BuildArgumentsString = str; }

	inline const std::string & buildToolPath() const { return m_BuildToolPath; }
	inline void setBuildToolPath(const std::string & path) { m_BuildToolPath = path; }

	inline const std::string & buildWorkingDirectory() const { return m_BuildWorkingDirectory; }
	inline void setBuildWorkingDirectory(const std::string & dir) { m_BuildWorkingDirectory = dir; }

	inline bool passBuildSettingsInEnvironment() const { return m_PassBuildSettingsInEnvironment; }
	inline void setPassBuildSettingsInEnvironment(bool flag) { m_PassBuildSettingsInEnvironment = flag; }

	inline void addBuildPhase(XCodeBuildPhase * phase) { m_Phases.push_back(phase); }
	inline void addDependency(XCodeTargetDependency * dep) { m_Deps.push_back(dep); }

	std::string toString() const;

private:
	std::string m_Name;
	std::string m_BuildArgumentsString;
	std::string m_BuildToolPath;
	std::string m_BuildWorkingDirectory;
	std::string m_ProductName;
	bool m_PassBuildSettingsInEnvironment;
	XCodeConfigurationList * m_BuildConfigurationList;
	std::vector<XCodeBuildPhase *> m_Phases;
	std::vector<XCodeTargetDependency *> m_Deps;

	XCodeLegacyTarget();
	~XCodeLegacyTarget();

	friend class XCodeProject;
};

#endif
