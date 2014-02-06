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
#ifndef __ddbedd7482cba259d35ea764efac2f60__
#define __ddbedd7482cba259d35ea764efac2f60__

#include "xcode_object.h"

class XCodeProject;

extern const std::string FILETYPE_WRAPPER_APPLICATION;
extern const std::string FILETYPE_WRAPPER_FRAMEWORK;
extern const std::string FILETYPE_COMPILED_MACHO_EXECUTABLE;
extern const std::string FILETYPE_TEXT;
extern const std::string FILETYPE_TEXT_PLIST_XML;
extern const std::string FILETYPE_TEXT_PLIST_STRINGS;
extern const std::string FILETYPE_SOURCECODE_C;
extern const std::string FILETYPE_SOURCECODE_C_H;
extern const std::string FILETYPE_SOURCECODE_CPP_CPP;
extern const std::string FILETYPE_SOURCECODE_CPP_H;
extern const std::string FILETYPE_SOURCECODE_C_OBJC;
extern const std::string FILETYPE_SOURCECODE_CPP_OBJCPP;
extern const std::string FILETYPE_SOURCECODE_GLSL;
extern const std::string FILETYPE_SOURCECODE_JAVA;
extern const std::string FILETYPE_IMAGE_PNG;
extern const std::string FILETYPE_IMAGE_JPEG;
extern const std::string FILETYPE_FOLDER_ASSETCATALOG;

class XCodeFileReference : public XCodeObject
{
public:
	inline std::string objectName() const { return (m_Name.length() > 0 ? m_Name : m_Path); }

	inline const std::string & explicitFileType() const { return m_ExplicitFileType; }
	inline void setExplicitFileType(const std::string & fileType) { m_ExplicitFileType = fileType; }

	inline const std::string & lastKnownFileType() const { return m_LastKnownFileType; }
	inline void setLastKnownFileType(const std::string & fileType) { m_LastKnownFileType = fileType; }

	inline bool includeInIndex() const { return m_IncludeInIndex; }
	inline void setIncludeInIndex(bool flag) { m_IncludeInIndex = flag; }

	inline const std::string & name() const { return m_Name; }
	inline void setName(const std::string & name) { m_Name = name; }

	inline const std::string & path() const { return m_Path; }
	inline void setPath(const std::string & path) { m_Path = path; }

	inline const std::string & sourceTree() const { return m_SourceTree; }
	inline void setSourceTree(const std::string & tree) { m_SourceTree = tree; }

	std::string toString() const;

private:
	std::string m_ExplicitFileType;
	std::string m_LastKnownFileType;
	bool m_IncludeInIndex;
	std::string m_Name;
	std::string m_Path;
	std::string m_SourceTree;

	XCodeFileReference();
	~XCodeFileReference();

	friend class XCodeProject;
};

#endif
