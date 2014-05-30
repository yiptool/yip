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
#include "parse_util.h"
#include "../util/tinyxml-util/tinyxml-util.h"
#include "../util/cxx-util/cxx-util/fmt.h"
#include "../util/cxx-util/cxx-util/explode.h"
#include "../util/cxx-util/cxx-util/trim.h"
#include "../util/strtod/strtod.h"

#undef uiScaleModeFromAttr1
#undef uiScaleModeFromAttr2
#undef uiScaleModeFromAttr4

void uiFloatPairFromAttr(const TiXmlAttribute * attr, float * outX, float * outY,
	float * outLandscapeX, float * outLandscapeY)
{
	std::vector<std::string> lists = explode(attr->ValueStr(), '/');
	if (lists.size() < 1 || lists.size() > 2)
		throw std::runtime_error(xmlInvalidAttributeValue(attr));

	std::vector<std::string> p = explode(lists[0], ',');
	std::vector<std::string> l = explode(lists.size() > 1 ? lists[1] : lists[0], ',');
	if (p.size() != 2 || l.size() != 2)
		throw std::runtime_error(xmlInvalidAttributeValue(attr));

	if (!strToFloat(trim(p[0]), *outX) || !strToFloat(trim(p[1]), *outY) ||
			!strToFloat(trim(l[0]), *outLandscapeX) || !strToFloat(trim(l[1]), *outLandscapeY))
		throw std::runtime_error(xmlInvalidAttributeValue(attr));
}

static void uiScaleModeFromAttr(const TiXmlAttribute * attr, UIScaleMode * outMode1, UIScaleMode * outMode2)
{
	std::vector<std::string> list = explode(attr->ValueStr(), '/');
	if (list.size() < 1 || list.size() > 2)
		throw std::runtime_error(xmlInvalidAttributeValue(attr));

	try
	{
		*outMode1 = uiScaleModeFromString(list[0]);
		*outMode2 = uiScaleModeFromString(list.size() > 1 ? list[1] : list[0]);
	}
	catch (const std::exception & e)
	{
		throw std::runtime_error(xmlError(attr, e.what()));
	}
}

void uiScaleModeFromAttr1(const TiXmlAttribute * attr, UIScaleMode * outMode1, UIScaleMode * outMode2,
	bool * hasValue)
{
	if (*hasValue)
		throw std::runtime_error(xmlError(attr, "multiple scale modes for the same attribute."));
	uiScaleModeFromAttr(attr, outMode1, outMode2);
	*hasValue = true;
}

void uiScaleModeFromAttr2(const TiXmlAttribute * attr, UIScaleMode * outModeA1, UIScaleMode * outModeA2,
	UIScaleMode * outModeB1, UIScaleMode * outModeB2, bool * hasValueA, bool * hasValueB)
{
	if (*hasValueA || *hasValueB)
		throw std::runtime_error(xmlError(attr, "multiple scale modes for the same attribute."));

	uiScaleModeFromAttr(attr, outModeA1, outModeA2);
	*outModeB1 = *outModeA1;
	*outModeB2 = *outModeA2;

	*hasValueA = true;
	*hasValueB = true;
}

void uiScaleModeFromAttr4(const TiXmlAttribute * attr, UIScaleMode * outModeA1, UIScaleMode * outModeA2,
	UIScaleMode * outModeB1, UIScaleMode * outModeB2, UIScaleMode * outModeC1, UIScaleMode * outModeC2,
	UIScaleMode * outModeD1, UIScaleMode * outModeD2, bool * hasValueA, bool * hasValueB, bool * hasValueC,
	bool * hasValueD)
{
	if (*hasValueA || *hasValueB || *hasValueC || *hasValueD)
		throw std::runtime_error(xmlError(attr, "multiple scale modes for the same attribute."));

	uiScaleModeFromAttr(attr, outModeA1, outModeA2);
	*outModeB1 = *outModeA1;
	*outModeB2 = *outModeA2;
	*outModeC1 = *outModeA1;
	*outModeC2 = *outModeA2;
	*outModeD1 = *outModeA1;
	*outModeD2 = *outModeA2;

	*hasValueA = true;
	*hasValueB = true;
	*hasValueC = true;
	*hasValueD = true;
}

void uiAlignmentFromAttr(const TiXmlAttribute * attr, UIAlignment * outAlign1, UIAlignment * outAlign2)
{
	std::vector<std::string> list = explode(attr->ValueStr(), '/');
	if (list.size() < 1 || list.size() > 2)
		throw std::runtime_error(xmlInvalidAttributeValue(attr));

	try
	{
		*outAlign1 = uiAlignmentFromString(list[0]);
		*outAlign2 = uiAlignmentFromString(list.size() > 1 ? list[1] : list[0]);
	}
	catch (const std::exception & e)
	{
		throw std::runtime_error(xmlError(attr, e.what()));
	}
}
