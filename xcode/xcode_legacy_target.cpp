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
#include "xcode_legacy_target.h"
#include "xcode_configuration_list.h"
#include "xcode_build_phase.h"
#include "xcode_target_dependency.h"
#include <sstream>

XCodeLegacyTarget::XCodeLegacyTarget()
	: XCodeObject("PBXLegacyTarget"),
	  m_BuildArgumentsString("$(ACTION)"),
	  m_BuildToolPath("/usr/bin/make"),
	  m_PassBuildSettingsInEnvironment(true),
	  m_BuildConfigurationList(nullptr)
{
}

XCodeLegacyTarget::~XCodeLegacyTarget()
{
}

std::string XCodeLegacyTarget::toString() const
{
	std::stringstream ss;

	ss << "\t\t" << objectID(this) << " = {\n";
	ss << "\t\t\tisa = " << className() << ";\n";

	ss << "\t\t\tbuildArgumentsString = " << stringLiteral(m_BuildArgumentsString) << ";\n";
	ss << "\t\t\tbuildConfigurationList = " << objectID(m_BuildConfigurationList) << ";\n";
	ss << "\t\t\tbuildPhases = (\n";
	for (std::vector<XCodeBuildPhase *>::const_iterator it = m_Phases.begin(); it != m_Phases.end(); ++it)
		ss << "\t\t\t\t" << objectID(*it) << ",\n";
	ss << "\t\t\t);\n";
	ss << "\t\t\tbuildToolPath = " << stringLiteral(m_BuildToolPath) << ";\n";
	if (m_BuildWorkingDirectory.length() > 0)
		ss << "\t\t\tbuildWorkingDirectory = " << stringLiteral(m_BuildWorkingDirectory) << ";\n";
	ss << "\t\t\tdependencies = (\n";
	for (std::vector<XCodeTargetDependency *>::const_iterator it = m_Deps.begin(); it != m_Deps.end(); ++it)
		ss << "\t\t\t\t" << objectID(*it) << ",\n";
	ss << "\t\t\t);\n";
	if (m_Name.length() > 0)
		ss << "\t\t\tname = " << stringLiteral(m_Name) << ";\n";
	ss << "\t\t\tpassBuildSettingsInEnvironment = " << (m_PassBuildSettingsInEnvironment ? 1 : 0) << ";\n";
	if (m_ProductName.length() > 0)
		ss << "\t\t\tproductName = " << stringLiteral(m_ProductName) << ";\n";

	ss << "\t\t};\n";

	return ss.str();
}
