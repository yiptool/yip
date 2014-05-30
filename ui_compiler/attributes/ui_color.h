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
#ifndef __5d64acae7bac59223f6689176f4baf74__
#define __5d64acae7bac59223f6689176f4baf74__

#include "../../3rdparty/tinyxml/tinyxml.h"
#include <string>
#include <unordered_map>

struct UIColor
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	static const UIColor clear;
	static const UIColor black;
	static const UIColor darkGray;
	static const UIColor lightGray;
	static const UIColor white;
	static const UIColor gray;
	static const UIColor red;
	static const UIColor green;
	static const UIColor blue;
	static const UIColor cyan;
	static const UIColor yellow;
	static const UIColor magenta;
	static const UIColor orange;
	static const UIColor purple;
	static const UIColor brown;

	static const std::unordered_map<std::string, UIColor> names;

	inline UIColor(float R, float G, float B, float A) noexcept
		: r(static_cast<unsigned char>(R * 255.0f)),
		  g(static_cast<unsigned char>(G * 255.0f)),
		  b(static_cast<unsigned char>(B * 255.0f)),
		  a(static_cast<unsigned char>(A * 255.0f))
	{
	}

	// In ios_generator.cpp
	std::string iosValue() const;

	inline bool operator==(const UIColor & c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }

	static UIColor fromString(const std::string & str);
	static UIColor fromAttr(const TiXmlAttribute * attr);
};

#endif
