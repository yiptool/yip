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
#include "xcode_project.h"
#include <sstream>

XCodeProject::XCodeProject()
	: XCodeObject("PBXProject"),
	  m_OrganizationName("Nikolay Zapolnov"),
	  m_DevelopmentRegion("English"),
	  m_MainGroup(NULL)
{
	m_KnownRegions.insert("en");
}

XCodeProject::~XCodeProject()
{
	for (std::vector<XCodeGroup *>::const_iterator it = m_Groups.begin(); it != m_Groups.end(); ++it)
		delete *it;

	for (std::vector<XCodeNativeTarget *>::const_iterator
			it = m_NativeTargets.begin(); it != m_NativeTargets.end(); ++it)
		delete *it;
}

XCodeGroup * XCodeProject::addGroup()
{
	XCodeGroup * group = new XCodeGroup;
	m_Groups.push_back(group);
	return group;
}

std::string XCodeProject::toString() const
{
	std::stringstream ss;
	ss << "// !$*UTF8*$!\n";
	ss << "{\n";
	ss << "\tarchiveVersion = 1;\n";
	ss << "\tclasses = {\n";
	ss << "\t};\n";
	ss << "\tobjectVersion = 46;\n";
	ss << "\tobjects = {\n";

	ss << "/* Begin PBXGroup section */\n";
	for (std::vector<XCodeGroup *>::const_iterator it = m_Groups.begin(); it != m_Groups.end(); ++it)
		ss << (*it)->toString();
	ss << "/* End PBXGroup section */\n";

	ss << "/* Begin PBXProject section */\n";
	ss << "\t\t" << uniqueID().toString() << " /* Project object */ = {\n";
	ss << "\t\t\tisa = " << className() << ";\n";
	ss << "\t\t\tattributes = {\n";
	ss << "\t\t\t\tLastUpgradeCheck = 0500;\n";
	ss << "\t\t\t\tORGANIZATIONNAME = " << stringLiteral(m_OrganizationName) << ";\n";
	ss << "\t\t\t};\n";
//	ss << "\t\t\tbuildConfigurationList = 0AB4FEFD17968C1B00105C66 /* Build configuration list for PBXProject \"Minesweeper\" */;\n"
	ss << "\t\t\tcompatibilityVersion = \"Xcode 3.2\";\n";
	ss << "\t\t\tdevelopmentRegion = " << stringLiteral(m_DevelopmentRegion) << ";\n";
	ss << "\t\t\thasScannedForEncodings = 0;\n";
	ss << "\t\t\tknownRegions = (\n";
	for (std::set<std::string>::const_iterator it = m_KnownRegions.begin(); it != m_KnownRegions.end(); ++it)
		ss << "\t\t\t\t" << stringLiteral(*it) << ",\n";
	ss << "\t\t\t);\n";
	if (m_MainGroup)
		ss << "\t\t\tmainGroup = " << m_MainGroup->uniqueID().toString() << ";\n";
//	ss << "\t\t\tproductRefGroup = 0AB4FF0317968C1B00105C66 /* Products */;\n";
	ss << "\t\t\tprojectDirPath = \"\";\n";
	ss << "\t\t\tprojectRoot = \"\";\n";
	ss << "\t\t\ttargets = (\n";
//	ss << "\t\t\t\t0AB4FF0117968C1B00105C66 /* Minesweeper */,
	ss << "\t\t\t);\n";
	ss << "\t\t};\n";
	ss << "/* End PBXProject section */\n";

	ss << "\t};\n";
	ss << "\trootObject = " << uniqueID().toString() << " /* Project object */;\n";
	ss << "}\n";
	return ss.str();
}
