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
#ifndef __b7390afae6562371d80d6ad21e31a2c6__
#define __b7390afae6562371d80d6ad21e31a2c6__

#include "xcode_object.h"
#include <vector>

class XCodeProject;
class XCodeBuildFile;

class XCodeBuildPhase : public XCodeObject
{
public:
	enum Type
	{
		Frameworks,
		Sources,
		Resources,
		ShellScript
	};

	inline std::string objectName() const { return phaseName(m_Type); }
	inline Type type() const { return m_Type; }

	inline long buildActionMask() const { return m_BuildActionMask; }
	inline void setBuildActionMask(long mask) { m_BuildActionMask = mask; }

	inline bool runOnlyForDeploymentPostProcessing() const { return m_RunOnlyForDeploymentPostProcessing; }
	inline void setRunOnlyForDeploymentPostProcessing(bool flag) { m_RunOnlyForDeploymentPostProcessing = flag; }

	inline const std::string & shellPath() const { return m_ShellPath; }
	inline void setShellPath(const std::string & path) { m_ShellPath = path; }

	inline const std::string & shellScript() const { return m_ShellScript; }
	inline void setShellScript(const std::string & script) { m_ShellScript = script; }

	inline bool showEnvVarsInLog() const { return m_ShowEnvVarsInLog; }
	inline void setShowEnvVarsInLog(bool flag) { m_ShowEnvVarsInLog = flag; }

	XCodeBuildFile * addFile();

	std::string toString() const;

private:
	XCodeProject * m_Project;
	Type m_Type;
	long m_BuildActionMask;
	bool m_RunOnlyForDeploymentPostProcessing;
	std::vector<XCodeBuildFile *> m_Files;
	std::string m_ShellPath;
	std::string m_ShellScript;
	bool m_ShowEnvVarsInLog;

	XCodeBuildPhase(XCodeProject * project, Type projectType);
	~XCodeBuildPhase();

	static std::string phaseName(Type type);
	static std::string classNameForPhase(Type type);

	friend class XCodeProject;
};

#endif
