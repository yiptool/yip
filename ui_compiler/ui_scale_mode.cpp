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
#include "ui_scale_mode.h"
#include "../util/tinyxml-util/tinyxml-util.h"
#include "../util/cxx-util/cxx-util/fmt.h"
#include <stdexcept>
#include <unordered_map>

static const std::unordered_map<std::string, UIScaleMode> g_ScaleModeNames = {
	{ "default", UIScaleDefault },
	{ "min", UIScaleMin },
	{ "max", UIScaleMax },
	{ "horz", UIScaleHorz },
	{ "vert", UIScaleVert },
	{ "avg", UIScaleAvg },
};

UIScaleMode uiScaleModeFromString(const std::string & str)
{
	auto it = g_ScaleModeNames.find(str);
	if (it == g_ScaleModeNames.end())
		throw std::runtime_error(fmt() << "invalid scale mode '" << str << "'.");
	return it->second;
}

UIScaleMode uiScaleModeFromAttr(const TiXmlAttribute * attr)
{
	auto it = g_ScaleModeNames.find(attr->ValueStr());
	if (it == g_ScaleModeNames.end())
		throw std::runtime_error(xmlError(attr, fmt() << "invalid scale mode '" << attr->ValueStr() << "'."));
	return it->second;
}
