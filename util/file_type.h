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
#ifndef __b4d24eb182097ba8c32f3c40e1889088__
#define __b4d24eb182097ba8c32f3c40e1889088__

#include <string>

enum FileType
{
	FILE_UNKNOWN = 0,
	FILE_TEXT,
	FILE_HTML,
	FILE_CSS,
	FILE_MARKDOWN,
	FILE_XML,
	FILE_ARCHIVE_ARJ,
	FILE_ARCHIVE_ZIP,
	FILE_ARCHIVE_RAR,
	FILE_ARCHIVE_TAR,
	FILE_ARCHIVE_TAR_GZ,
	FILE_ARCHIVE_TAR_BZ2,
	FILE_ARCHIVE_GZIP,
	FILE_ARCHIVE_BZIP2,
	FILE_BUILD_MAKEFILE,
	FILE_BUILD_CMAKE_SCRIPT,
	FILE_BUILD_CMAKELISTS,
	FILE_SOURCE_C,
	FILE_SOURCE_CXX,
	FILE_SOURCE_C_HEADER,
	FILE_SOURCE_CXX_HEADER,
	FILE_SOURCE_OBJC,
	FILE_SOURCE_OBJCXX,
	FILE_SOURCE_JAVA,
	FILE_SOURCE_JAVASCRIPT,
	FILE_SOURCE_LUA,
	FILE_IMAGE_PNG,
	FILE_IMAGE_JPEG,
	FILE_IMAGE_GIF,
	FILE_IMAGE_BMP,
};

FileType determineFileType(const std::string & path);

#endif
