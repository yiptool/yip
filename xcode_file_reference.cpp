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
#include "xcode_file_reference.h"
#include <sstream>

const std::string FILETYPE_WRAPPER_APPLICATION = "wrapper.application";
const std::string FILETYPE_WRAPPER_FRAMEWORK = "wrapper.framework";
const std::string FILETYPE_TEXT = "text";
const std::string FILETYPE_TEXT_PLIST_XML = "text.plist.xml";
const std::string FILETYPE_TEXT_PLIST_STRINGS = "text.plist.strings";
const std::string FILETYPE_SOURCECODE_C = "sourcecode.c";
const std::string FILETYPE_SOURCECODE_C_H = "sourcecode.c.h";
const std::string FILETYPE_SOURCECODE_CPP_CPP = "sourcecode.cpp.cpp";
const std::string FILETYPE_SOURCECODE_CPP_H = "sourcecode.cpp.h";
const std::string FILETYPE_SOURCECODE_C_OBJC = "sourcecode.c.objc";
const std::string FILETYPE_SOURCECODE_CPP_OBJCPP = "sourcecode.cpp.objcpp";
const std::string FILETYPE_SOURCECODE_GLSL = "sourcecode.glsl";
const std::string FILETYPE_SOURCECODE_JAVA = "sourcecode.java";
const std::string FILETYPE_IMAGE_PNG = "image.png";
const std::string FILETYPE_IMAGE_JPEG = "image.jpeg";
const std::string FILETYPE_FOLDER_ASSETCATALOG = "folder.assetcatalog";

XCodeFileReference::XCodeFileReference()
	: XCodeObject("PBXFileReference"),
	  m_IncludeInIndex(true),
	  m_SourceTree("<group>")
{
}

XCodeFileReference::~XCodeFileReference()
{
}

std::string XCodeFileReference::toString() const
{
	std::stringstream ss;

	ss << objectID(this) << " = {";
	ss << "isa = " << className() << "; ";

	if (m_ExplicitFileType.length() > 0)
		ss << "explicitFileType = " << stringLiteral(m_ExplicitFileType) << "; ";

	if (m_LastKnownFileType.length() > 0)
		ss << "lastKnownFileType = " << stringLiteral(m_LastKnownFileType) << "; ";

	if (!m_IncludeInIndex)
		ss << "includeInIndex = 0; ";

	if (m_Name.length() > 0)
		ss << "name = " << stringLiteral(m_Name) << "; ";

	if (m_Path.length() > 0)
		ss << "path = " << stringLiteral(m_Path) << "; ";

	if (m_SourceTree.length() > 0)
		ss << "sourceTree = " << stringLiteral(m_SourceTree) << "; ";

	ss << "};";

	return ss.str();
}
