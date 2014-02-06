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
#include "xcode_target_build_configuration.h"

XCodeTargetBuildConfiguration::XCodeTargetBuildConfiguration()
	: m_PrecompilePrefixHeader(false),
	  m_ProductName("$(TARGET_NAME)"),
	  m_WrapperExtension("app")
{
}

XCodeTargetBuildConfiguration::~XCodeTargetBuildConfiguration()
{
}

void XCodeTargetBuildConfiguration::writeBuildSettings(std::stringstream & ss) const
{
	if (m_AssetCatalogAppIconName.length() > 0)
		ss << "\t\t\t\tASSETCATALOG_COMPILER_APPICON_NAME = " << stringLiteral(m_AssetCatalogAppIconName) << ";\n";
	if (m_AssetCatalogLaunchImageName.length() > 0)
		ss << "\t\t\t\tASSETCATALOG_COMPILER_LAUNCHIMAGE_NAME = " << stringLiteral(m_AssetCatalogLaunchImageName) << ";\n";
	ss << "\t\t\t\tGCC_PRECOMPILE_PREFIX_HEADER = " << (m_PrecompilePrefixHeader ? "YES" : "NO") << ";\n";
	if (m_PrefixHeader.length() > 0)
		ss << "\t\t\t\tGCC_PREFIX_HEADER = " << stringLiteral(m_PrefixHeader) << ";\n";
	if (m_InfoPListFile.length() > 0)
		ss << "\t\t\t\tINFOPLIST_FILE = " << stringLiteral(m_InfoPListFile) << ";\n";
	if (m_ProductName.length() > 0)
		ss << "\t\t\t\tPRODUCT_NAME = " << stringLiteral(m_ProductName) << ";\n";
	if (m_WrapperExtension.length() > 0)
		ss << "\t\t\t\tWRAPPER_EXTENSION = " << stringLiteral(m_WrapperExtension) << ";\n";
}
