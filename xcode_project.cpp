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
	  m_MainGroup(NULL),
	  m_ProductRefGroup(NULL),
	  m_BuildConfigurationList(NULL)
{
	m_KnownRegions.insert("en");
}

XCodeProject::~XCodeProject()
{
	for (std::vector<XCodeGroup *>::const_iterator it = m_Groups.begin(); it != m_Groups.end(); ++it)
		delete *it;

	for (std::vector<XCodeVariantGroup *>::const_iterator it = m_VarGroups.begin(); it != m_VarGroups.end(); ++it)
		delete *it;

	for (std::vector<XCodeFileReference *>::const_iterator it = m_FileRefs.begin(); it != m_FileRefs.end(); ++it)
		delete *it;

	for (std::vector<XCodeBuildConfiguration *>::const_iterator it = m_Cfgs.begin(); it != m_Cfgs.end(); ++it)
		delete *it;

	for (std::vector<XCodeConfigurationList *>::const_iterator it = m_CfgLists.begin(); it != m_CfgLists.end(); ++it)
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

XCodeVariantGroup * XCodeProject::addVariantGroup()
{
	XCodeVariantGroup * group = new XCodeVariantGroup;
	m_VarGroups.push_back(group);
	return group;
}

XCodeFileReference * XCodeProject::addFileReference()
{
	XCodeFileReference * ref = new XCodeFileReference;
	m_FileRefs.push_back(ref);
	return ref;
}

XCodeBuildConfiguration * XCodeProject::addBuildConfiguration()
{
	XCodeBuildConfiguration * config = new XCodeBuildConfiguration;
	m_Cfgs.push_back(config);
	return config;
}

XCodeConfigurationList * XCodeProject::addConfigurationList()
{
	XCodeConfigurationList * list = new XCodeConfigurationList;
	m_CfgLists.push_back(list);
	return list;
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

	ss << "/* Begin PBXFileReference section */\n";
	for (std::vector<XCodeFileReference *>::const_iterator it = m_FileRefs.begin(); it != m_FileRefs.end(); ++it)
		ss << "\t\t" << (*it)->toString() << '\n';
	ss << "/* End PBXFileReference section */\n";
	ss << '\n';

	ss << "/* Begin PBXGroup section */\n";
	for (std::vector<XCodeGroup *>::const_iterator it = m_Groups.begin(); it != m_Groups.end(); ++it)
		ss << (*it)->toString();
	ss << "/* End PBXGroup section */\n";
	ss << '\n';

	ss << "/* Begin PBXProject section */\n";
	ss << "\t\t" << objectID(this) << " = {\n";
	ss << "\t\t\tisa = " << className() << ";\n";
	ss << "\t\t\tattributes = {\n";
	ss << "\t\t\t\tLastUpgradeCheck = 0500;\n";
	ss << "\t\t\t\tORGANIZATIONNAME = " << stringLiteral(m_OrganizationName) << ";\n";
	ss << "\t\t\t};\n";
	if (m_BuildConfigurationList)
		ss << "\t\t\tbuildConfigurationList = " << objectID(m_BuildConfigurationList) << ";\n";
	ss << "\t\t\tcompatibilityVersion = \"Xcode 3.2\";\n";
	ss << "\t\t\tdevelopmentRegion = " << stringLiteral(m_DevelopmentRegion) << ";\n";
	ss << "\t\t\thasScannedForEncodings = 0;\n";
	ss << "\t\t\tknownRegions = (\n";
	for (std::set<std::string>::const_iterator it = m_KnownRegions.begin(); it != m_KnownRegions.end(); ++it)
		ss << "\t\t\t\t" << stringLiteral(*it) << ",\n";
	ss << "\t\t\t);\n";
	if (m_MainGroup)
		ss << "\t\t\tmainGroup = " << objectID(m_MainGroup) << ";\n";
	if (m_ProductRefGroup)
		ss << "\t\t\tproductRefGroup = " << objectID(m_ProductRefGroup) << ";\n";
	ss << "\t\t\tprojectDirPath = " << stringLiteral(m_ProjectDirPath) << ";\n";
	ss << "\t\t\tprojectRoot = " << stringLiteral(m_ProjectRoot) << ";\n";
	ss << "\t\t\ttargets = (\n";
//	ss << "\t\t\t\t0AB4FF0117968C1B00105C66 /* Minesweeper */,
	ss << "\t\t\t);\n";
	ss << "\t\t};\n";
	ss << "/* End PBXProject section */\n";

	ss << "/* Begin PBXVariantGroup section */\n";
	for (std::vector<XCodeVariantGroup *>::const_iterator it = m_VarGroups.begin(); it != m_VarGroups.end(); ++it)
		ss << (*it)->toString();
	ss << "/* End PBXVariantGroup section */\n";
	ss << '\n';

	ss << "/* Begin XCBuildConfiguration section */\n";
	for (std::vector<XCodeBuildConfiguration *>::const_iterator it = m_Cfgs.begin(); it != m_Cfgs.end(); ++it)
		ss << (*it)->toString();
	ss << "/* End XCBuildConfiguration section */\n";
	ss << '\n';

	ss << "/* Begin XCConfigurationList section */\n";
	for (std::vector<XCodeConfigurationList *>::const_iterator it = m_CfgLists.begin(); it != m_CfgLists.end(); ++it)
		ss << (*it)->toString();
	ss << "/* End XCConfigurationList section */\n";

	ss << "\t};\n";
	ss << "\trootObject = " << uniqueID().toString() << " /* Project object */;\n";
	ss << "}\n";
	return ss.str();
}
