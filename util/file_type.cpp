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
#include "file_type.h"
#include "path.h"
#include <unordered_map>

static std::unordered_map<std::string, FileType> g_ExtMap;
static std::unordered_map<std::string, FileType> g_NameMap;
static bool g_Initialized;

FileType determineFileType(const std::string & path)
{
	if (!g_Initialized)
	{
		g_ExtMap.insert(std::make_pair(".txt", FILE_TEXT_PLAIN));
		g_ExtMap.insert(std::make_pair(".htm", FILE_TEXT_HTML));
		g_ExtMap.insert(std::make_pair(".html", FILE_TEXT_HTML));
		g_ExtMap.insert(std::make_pair(".css", FILE_TEXT_CSS));
		g_ExtMap.insert(std::make_pair(".md", FILE_TEXT_MARKDOWN));
		g_ExtMap.insert(std::make_pair(".xml", FILE_TEXT_XML));
		g_ExtMap.insert(std::make_pair(".arj", FILE_ARCHIVE_ARJ));
		g_ExtMap.insert(std::make_pair(".zip", FILE_ARCHIVE_ZIP));
		g_ExtMap.insert(std::make_pair(".rar", FILE_ARCHIVE_RAR));
		g_ExtMap.insert(std::make_pair(".tar", FILE_ARCHIVE_TAR));
		g_ExtMap.insert(std::make_pair(".tar.gz", FILE_ARCHIVE_TAR_GZ));
		g_ExtMap.insert(std::make_pair(".tar.bz2", FILE_ARCHIVE_TAR_BZ2));
		g_ExtMap.insert(std::make_pair(".gz", FILE_ARCHIVE_GZIP));
		g_ExtMap.insert(std::make_pair(".bz2", FILE_ARCHIVE_BZIP2));
		g_ExtMap.insert(std::make_pair(".cmake", FILE_BUILD_CMAKE_SCRIPT));
		g_ExtMap.insert(std::make_pair(".exe", FILE_BINARY_WIN32_EXE));
		g_ExtMap.insert(std::make_pair(".dll", FILE_BINARY_WIN32_DLL));
		g_ExtMap.insert(std::make_pair(".res", FILE_BINARY_WIN32_RES));
		g_ExtMap.insert(std::make_pair(".lib", FILE_BINARY_WIN32_LIB));
		g_ExtMap.insert(std::make_pair(".o", FILE_BINARY_OBJECT));
		g_ExtMap.insert(std::make_pair(".obj", FILE_BINARY_OBJECT));
		g_ExtMap.insert(std::make_pair(".so", FILE_BINARY_SHARED_OBJECT));
		g_ExtMap.insert(std::make_pair(".a", FILE_BINARY_ARCHIVE));
		g_ExtMap.insert(std::make_pair(".framework", FILE_BINARY_APPLE_FRAMEWORK));
		g_ExtMap.insert(std::make_pair(".c", FILE_SOURCE_C));
		g_ExtMap.insert(std::make_pair(".C", FILE_SOURCE_CXX));
		g_ExtMap.insert(std::make_pair(".cc", FILE_SOURCE_CXX));
		g_ExtMap.insert(std::make_pair(".cpp", FILE_SOURCE_CXX));
		g_ExtMap.insert(std::make_pair(".h", FILE_SOURCE_C_HEADER));
		g_ExtMap.insert(std::make_pair(".H", FILE_SOURCE_CXX_HEADER));
		g_ExtMap.insert(std::make_pair(".hh", FILE_SOURCE_CXX_HEADER));
		g_ExtMap.insert(std::make_pair(".hpp", FILE_SOURCE_CXX_HEADER));
		g_ExtMap.insert(std::make_pair(".m", FILE_SOURCE_OBJC));
		g_ExtMap.insert(std::make_pair(".mm", FILE_SOURCE_OBJCXX));
		g_ExtMap.insert(std::make_pair(".java", FILE_SOURCE_JAVA));
		g_ExtMap.insert(std::make_pair(".js", FILE_SOURCE_JAVASCRIPT));
		g_ExtMap.insert(std::make_pair(".lua", FILE_SOURCE_LUA));
		g_ExtMap.insert(std::make_pair(".sh", FILE_SOURCE_SHELL));
		g_ExtMap.insert(std::make_pair(".php", FILE_SOURCE_PHP));
		g_ExtMap.insert(std::make_pair(".php4", FILE_SOURCE_PHP));
		g_ExtMap.insert(std::make_pair(".php5", FILE_SOURCE_PHP));
		g_ExtMap.insert(std::make_pair(".py", FILE_SOURCE_PYTHON));
		g_ExtMap.insert(std::make_pair(".rc", FILE_SOURCE_WIN32_RC));
		g_ExtMap.insert(std::make_pair(".def", FILE_SOURCE_WIN32_DEF));
		g_ExtMap.insert(std::make_pair(".plist", FILE_SOURCE_PLIST));
		g_ExtMap.insert(std::make_pair(".xcassets", FILE_IMAGE_XCASSETS));
		g_ExtMap.insert(std::make_pair(".png", FILE_IMAGE_PNG));
		g_ExtMap.insert(std::make_pair(".jpg", FILE_IMAGE_JPEG));
		g_ExtMap.insert(std::make_pair(".jpeg", FILE_IMAGE_JPEG));
		g_ExtMap.insert(std::make_pair(".gif", FILE_IMAGE_GIF));
		g_ExtMap.insert(std::make_pair(".bmp", FILE_IMAGE_BMP));
		g_NameMap.insert(std::make_pair("GNUmakefile", FILE_BUILD_MAKEFILE));
		g_NameMap.insert(std::make_pair("makefile", FILE_BUILD_MAKEFILE));
		g_NameMap.insert(std::make_pair("Makefile", FILE_BUILD_MAKEFILE));
		g_NameMap.insert(std::make_pair("CMakeLists.txt", FILE_BUILD_CMAKELISTS));
		g_Initialized = true;
	}

	std::string name = pathGetFileName(path);
	auto it = g_NameMap.find(name);
	if (it != g_NameMap.end())
		return it->second;

	std::string ext = pathGetFullFileExtension(path);
	auto jt = g_ExtMap.find(ext);
	if (jt != g_ExtMap.end())
		return jt->second;

	ext = pathGetShortFileExtension(path);
	jt = g_ExtMap.find(ext);
	if (jt != g_ExtMap.end())
		return jt->second;

	return FILE_UNKNOWN;
}
