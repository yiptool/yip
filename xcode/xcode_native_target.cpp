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
#include "xcode_native_target.h"
#include "xcode_configuration_list.h"
#include "xcode_build_phase.h"
#include "xcode_target_dependency.h"
#include <sstream>

const std::string PRODUCTTYPE_APPLICATION = "com.apple.product-type.application";
const std::string PRODUCTTYPE_TOOL = "com.apple.product-type.tool";

XCodeNativeTarget::XCodeNativeTarget()
	: XCodeObject("PBXNativeTarget"),
	  m_BuildConfigurationList(nullptr),
	  m_ProductReference(nullptr),
	  m_ProductType(PRODUCTTYPE_APPLICATION)
{
}

XCodeNativeTarget::~XCodeNativeTarget()
{
}

std::string XCodeNativeTarget::toString() const
{
	std::stringstream ss;

	ss << "\t\t" << objectID(this) << " = {\n";
	ss << "\t\t\tisa = " << className() << ";\n";
	ss << "\t\t\tbuildConfigurationList = " << objectID(m_BuildConfigurationList) << ";\n";
	ss << "\t\t\tbuildPhases = (\n";
	for (std::vector<XCodeBuildPhase *>::const_iterator it = m_Phases.begin(); it != m_Phases.end(); ++it)
		ss << "\t\t\t\t" << objectID(*it) << ",\n";
	ss << "\t\t\t);\n";
	ss << "\t\t\tbuildRules = (\n";
	ss << "\t\t\t);\n";
	ss << "\t\t\tdependencies = (\n";
	for (std::vector<XCodeTargetDependency *>::const_iterator it = m_Deps.begin(); it != m_Deps.end(); ++it)
		ss << "\t\t\t\t" << objectID(*it) << ",\n";
	ss << "\t\t\t);\n";

	if (m_Name.length() > 0)
		ss << "\t\t\tname = " << stringLiteral(m_Name) << ";\n";

	if (m_ProductName.length() > 0)
		ss << "\t\t\tproductName = " << stringLiteral(m_ProductName) << ";\n";

	if (m_ProductReference)
		ss << "\t\t\tproductReference = " << objectID(m_ProductReference) << ";\n";

	if (m_ProductType.length() > 0)
		ss << "\t\t\tproductType = " << stringLiteral(m_ProductType) << ";\n";

	ss << "\t\t};\n";

	return ss.str();
}
