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
#ifndef __dee757a372efbf0af613ed62448b8c05__
#define __dee757a372efbf0af613ed62448b8c05__

#include <string>

std::string pathToNativeSeparators(const std::string & path);
std::string pathToUnixSeparators(const std::string & path);

bool pathIsSeparator(char ch);
bool pathIsWin32DriveLetter(char ch);
bool pathIsWin32PathWithDriveLetter(const std::string & path);
bool pathIsUNC(const std::string & path);

size_t pathIndexOfFileName(const std::string & path);
std::string pathGetFileName(const std::string & path);

std::string pathGetShortFileExtension(const std::string & path);
std::string pathGetFullFileExtension(const std::string & path);

#endif