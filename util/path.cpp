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
#include "path.h"

std::string pathToNativeSeparators(const std::string & path)
{
  #ifndef _WIN32
	return path;
  #else
	std::string result = path;
	for (char & ch : result)
	{
		if (ch == '/')
			ch = '\\';
	}
	return result;
  #endif
}

std::string pathToUnixSeparators(const std::string & path)
{
  #ifndef _WIN32
	return path;
  #else
	std::string result = path;
	for (char & ch : result)
	{
		if (ch == '\\')
			ch = '/';
	}
	return result;
  #endif
}

bool pathIsSeparator(char ch)
{
	if (ch == '/')
		return true;

  #ifndef _WIN32
	return false;
  #else
	return ch == '\\';
  #endif
}

bool pathIsWin32DriveLetter(char ch)
{
	return ((ch >= 'a' && ch <= 'z') && (ch >= 'A' && ch <= 'Z'));
}

bool pathIsWin32PathWithDriveLetter(const std::string & path)
{
	return (path.length() >= 2 && path[1] == ':' && pathIsWin32DriveLetter(path[0]));
}

bool pathIsUNC(const std::string & path)
{
	return (path.length() >= 2 && pathIsSeparator(path[0]) && pathIsSeparator(path[1]));
}

size_t pathIndexOfBaseName(const std::string & path)
{
	size_t pos = path.rfind('/');

  #ifdef _WIN32
	size_t pos2 = path.rfind('\\');
	if (pos2 != std::string::npos && (pos == std::string::npos || pos2 > pos))
		pos = pos2;
	if (pos == std::string::npos && pathIsWin32PathWithDriveLetter(path))
		pos = 1;
  #endif

	return (pos != std::string::npos ? pos + 1 : 0);
}

std::string pathGetFileName(const std::string & path)
{
	return path.substr(pathIndexOfBaseName(path));
}

std::string pathGetShortFileExtension(const std::string & path)
{
	size_t pos = path.rfind('.');
	if (pos == std::string::npos)
		return std::string();

	size_t offset = pathIndexOfBaseName(path);
	if (pos < offset)
		return std::string();

	return path.substr(pos);
}

std::string pathGetFullFileExtension(const std::string & path)
{
	size_t offset = pathIndexOfBaseName(path);
	size_t pos = path.find('.', offset);
	return (pos == std::string::npos ? std::string() : path.substr(pos));
}
