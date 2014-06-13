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
#include "ui_color.h"
#include "../../util/tinyxml-util/tinyxml-util.h"
#include "../../util/cxx-util/cxx-util/fmt.h"
#include "../../util/cxx-util/cxx-util/unhex.h"
#include <stdexcept>
#include <unordered_map>
#include <sstream>

const UIColor UIColor::clear(0.0f, 0.0f, 0.0f, 0.0f);
const UIColor UIColor::black(0.0f, 0.0f, 0.0f, 1.0f);
const UIColor UIColor::darkGray(1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f, 1.0f);
const UIColor UIColor::lightGray(2.0f / 3.0f, 2.0f / 3.0f, 2.0f / 3.0f, 1.0f);
const UIColor UIColor::white(1.0f, 1.0f, 1.0f, 1.0f);
const UIColor UIColor::gray(0.5f, 0.5f, 0.5f, 1.0f);
const UIColor UIColor::red(1.0f, 0.0f, 0.0f, 1.0f);
const UIColor UIColor::green(0.0f, 1.0f, 0.0f, 1.0f);
const UIColor UIColor::blue(0.0f, 0.0f, 1.0f, 1.0f);
const UIColor UIColor::cyan(0.0f, 1.0f, 1.0f, 1.0f);
const UIColor UIColor::yellow(1.0f, 1.0f, 0.0f, 1.0f);
const UIColor UIColor::magenta(1.0f, 0.0f, 1.0f, 1.0f);
const UIColor UIColor::orange(1.0f, 0.5f, 0.0f, 1.0f);
const UIColor UIColor::purple(0.5f, 0.0f, 0.5f, 1.0f);
const UIColor UIColor::brown(0.6f, 0.4f, 0.2f, 1.0f);

const std::unordered_map<std::string, UIColor> UIColor::names = {
	{ "clear", UIColor::clear },
	{ "black", UIColor::black },
	{ "darkgray", UIColor::darkGray },
	{ "lightgray", UIColor::lightGray },
	{ "white", UIColor::white },
	{ "gray", UIColor::gray },
	{ "red", UIColor::red },
	{ "green", UIColor::green },
	{ "blue", UIColor::blue },
	{ "cyan", UIColor::cyan },
	{ "yellow", UIColor::yellow },
	{ "magenta", UIColor::magenta },
	{ "orange", UIColor::orange },
	{ "purple", UIColor::purple },
	{ "brown", UIColor::brown },
};

UIColor UIColor::fromString(const std::string & str)
{
	auto it = names.find(str);
	if (it != names.end())
		return it->second;

	if (str.length() > 0 && str[0] == '#')
	{
		if (str.length() == 4)
		{
			float R = unhex(str[1]) / 15.0f;
			float G = unhex(str[2]) / 15.0f;
			float B = unhex(str[3]) / 15.0f;
			return UIColor(R, G, B, 1.0f);
		}
		else if (str.length() == 5)
		{
			float A = unhex(str[1]) / 15.0f;
			float R = unhex(str[2]) / 15.0f;
			float G = unhex(str[3]) / 15.0f;
			float B = unhex(str[4]) / 15.0f;
			return UIColor(R, G, B, A);
		}
		else if (str.length() == 7)
		{
			float R = unhex2(str[1], str[2]) / 255.0f;
			float G = unhex2(str[3], str[4]) / 255.0f;
			float B = unhex2(str[5], str[6]) / 255.0f;
			return UIColor(R, G, B, 1.0f);
		}
		else if (str.length() == 9)
		{
			float A = unhex2(str[1], str[2]) / 255.0f;
			float R = unhex2(str[3], str[4]) / 255.0f;
			float G = unhex2(str[5], str[6]) / 255.0f;
			float B = unhex2(str[7], str[8]) / 255.0f;
			return UIColor(R, G, B, A);
		}
	}

	throw std::runtime_error(fmt() << "invalid color value '" << str << "'.");
}

UIColor UIColor::fromAttr(const TiXmlAttribute * attr)
{
	try
	{
		return fromString(attr->ValueStr());
	}
	catch (const std::exception & e)
	{
		throw std::runtime_error(xmlError(attr, e.what()));
	}
}
