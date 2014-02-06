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
#include "xcode_build_phase.h"
#include "xcode_project.h"
#include <sstream>
#include <stdexcept>

XCodeBuildPhase::XCodeBuildPhase(XCodeProject * project, Type type)
	: XCodeObject(classNameForPhase(type)),
	  m_Project(project),
	  m_Type(type),
	  m_BuildActionMask(2147483647),
	  m_RunOnlyForDeploymentPostProcessing(false),
	  m_ShellPath("/bin/sh"),
	  m_ShellScript("exit 0"),
	  m_ShowEnvVarsInLog(false)
{
}

XCodeBuildPhase::~XCodeBuildPhase()
{
}

XCodeBuildFile * XCodeBuildPhase::addFile()
{
	XCodeBuildFile * file = m_Project->addBuildFile(this);
	m_Files.push_back(file);
	return file;
}

std::string XCodeBuildPhase::toString() const
{
	std::stringstream ss;

	ss << "\t\t" << objectID(this) << " = {\n";
	ss << "\t\t\tisa = " << className() << ";\n";
	ss << "\t\t\tbuildActionMask = " << m_BuildActionMask << ";\n";
	ss << "\t\t\tfiles = (\n";
	for (std::vector<XCodeBuildFile *>::const_iterator it = m_Files.begin(); it != m_Files.end(); ++it)
		ss << "\t\t\t\t" << objectID(*it) << ",\n";
	ss << "\t\t\t);\n";
	if (m_Type == ShellScript)
	{
		ss << "\t\t\tinputPaths = (\n";
		ss << "\t\t\t);\n";
		ss << "\t\t\toutputPaths = (\n";
		ss << "\t\t\t);\n";
	}
	ss << "\t\t\trunOnlyForDeploymentPostprocessing = " << (m_RunOnlyForDeploymentPostProcessing ? 1 : 0) << ";\n";
	if (m_Type == ShellScript)
	{
		ss << "\t\t\tshellPath = " << stringLiteral(m_ShellPath) << ";\n";
		ss << "\t\t\tshellScript = " << stringLiteral(m_ShellScript) << ";\n";
		ss << "\t\t\tsnowEnvVarsInLog = " << (m_ShowEnvVarsInLog ? 1 : 0) << ";\n";
	}
	ss << "\t\t};\n";

	return ss.str();
}

std::string XCodeBuildPhase::phaseName(Type type)
{
	switch (type)
	{
	case Frameworks: return "Frameworks";
	case Sources: return "Sources";
	case Resources: return "Resources";
	case ShellScript: return "Shell Scripts";
	}
	throw std::runtime_error("invalid build phase.");
}

std::string XCodeBuildPhase::classNameForPhase(Type type)
{
	switch (type)
	{
	case Frameworks: return "PBXFrameworksBuildPhase";
	case Sources: return "PBXSourcesBuildPhase";
	case Resources: return "PBXResourcesBuildPhase";
	case ShellScript: return "PBXShellScriptBuildPhase";
	}
	throw std::runtime_error("invalid build phase.");
}
