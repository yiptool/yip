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
#ifndef __538c2b09160c3dd1fbf7f07e863a6990__
#define __538c2b09160c3dd1fbf7f07e863a6990__

#include "../../3rdparty/tinyxml/tinyxml.h"
#include <string>
#include <memory>

struct UIFont;
typedef std::shared_ptr<UIFont> UIFontPtr;

struct UIFont
{
	std::string family;
	float size;
	float landscapeSize;

	inline UIFont(const std::string & f, float s) : family(f), size(s), landscapeSize(s) {}
	inline UIFont(const std::string & f, float s, float ls) : family(f), size(s), landscapeSize(ls) {}

	static UIFontPtr fromString(const std::string & str);
};

#endif
