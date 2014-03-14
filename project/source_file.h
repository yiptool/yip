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
#ifndef __cd8034b5cabf20e1be1ba06b0fc20482__
#define __cd8034b5cabf20e1be1ba06b0fc20482__

#include "platform.h"
#include "../util/file_type.h"
#include <string>
#include <memory>

class SourceFile
{
public:
	SourceFile(const std::string & fileName, const std::string & filePath);
	~SourceFile();

	inline const std::string & name() const { return m_Name; }
	inline const std::string & path() const { return m_Path; }

	inline FileType type() const { return m_Type; }
	inline void setFileType(FileType type) { m_Type = type; }

	inline Platform::Type platforms() const { return m_Platforms; }
	inline void setPlatforms(Platform::Type flags) { m_Platforms = flags; }

private:
	std::string m_Name;
	std::string m_Path;
	FileType m_Type;
	Platform::Type m_Platforms;

	SourceFile(const SourceFile &) = delete;
	SourceFile & operator=(const SourceFile &) = delete;
};

typedef std::shared_ptr<SourceFile> SourceFilePtr;

#endif
