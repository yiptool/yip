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
#ifndef __fa594c356da31cf51f641b71cfedf275__
#define __fa594c356da31cf51f641b71cfedf275__

#include <string>

namespace Platform
{
	static const unsigned None = 0;
	static const unsigned All = 0xFFFF;

	static const unsigned iOS = 0x0001;
	static const unsigned Android = 0x0002;
	static const unsigned OSX = 0x0004;
	static const unsigned WinRT = 0x0008;
	static const unsigned Qt = 0x0010;
	static const unsigned Tizen = 0x0020;
	static const unsigned NaCl = 0x0040;

	typedef unsigned Type;
};

enum class BuildType
{
	Unspecified = 0,
	Debug,
	Release,
};

Platform::Type platformFromString(const std::string & name);
Platform::Type platformFromString(const std::string & name, const std::nothrow_t &);

BuildType buildTypeFromString(const std::string & name);
BuildType buildTypeFromString(const std::string & name, const std::nothrow_t &);

#endif
