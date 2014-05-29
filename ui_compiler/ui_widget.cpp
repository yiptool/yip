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
#include "ui_widget.h"
#include "ui_layout.h"
#include "ui_group.h"
#include "ui_button.h"
#include "ui_label.h"
#include "ui_image.h"
#include "../util/tinyxml-util/tinyxml-util.h"
#include "../util/cxx-util/cxx-util/fmt.h"
#include "../util/cxx-util/cxx-util/explode.h"
#include "../util/cxx-util/cxx-util/trim.h"
#include "../util/strtod/strtod.h"
#include <cassert>

UIWidget::UIWidget(UILayout * layout, UIGroup * parentGroup, Kind kind)
	: m_Layout(layout),
	  m_Parent(parentGroup),
	  m_Kind(kind),
	  m_ID(fmt() << "_widget" << m_Layout->m_NextUniqueID++),
	  m_BackgroundColor(UIColor::clear),
	  m_X(0.0f),
	  m_Y(0.0f),
	  m_Width(0.0f),
	  m_Height(0.0f),
	  m_LandscapeX(0.0f),
	  m_LandscapeY(0.0f),
	  m_LandscapeWidth(0.0f),
	  m_LandscapeHeight(0.0f),
	  m_XScaleMode(UIScaleDefault),
	  m_YScaleMode(UIScaleDefault),
	  m_WidthScaleMode(UIScaleDefault),
	  m_HeightScaleMode(UIScaleDefault),
	  m_LandscapeXScaleMode(UIScaleDefault),
	  m_LandscapeYScaleMode(UIScaleDefault),
	  m_LandscapeWidthScaleMode(UIScaleDefault),
	  m_LandscapeHeightScaleMode(UIScaleDefault),
	  m_Alignment(UIAlignUnspecified),
	  m_LandscapeAlignment(UIAlignUnspecified)
{
}

UIWidget::~UIWidget()
{
}

UIWidgetPtr UIWidget::create(UILayout * layout, UIGroup * parentGroup, const std::string & className)
{
	if (className == "group")
		return std::make_shared<UIGroup>(layout, parentGroup);
	else if (className == "button")
		return std::make_shared<UIButton>(layout, parentGroup);
	else if (className == "label")
		return std::make_shared<UILabel>(layout, parentGroup);
	else if (className == "image")
		return std::make_shared<UIImage>(layout, parentGroup);

	throw std::runtime_error(fmt() << "invalid widget class '" << className << "'.");
}

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

static void uiScaleModeFromAttr1(const TiXmlAttribute * attr, UIScaleMode * outMode1, UIScaleMode * outMode2,
	bool * hasValue)
{
	if (*hasValue)
		throw std::runtime_error(xmlError(attr, "multiple scale modes for the same attribute."));
	uiScaleModeFromAttr(attr, outMode1, outMode2);
	*hasValue = true;
}

static void uiScaleModeFromAttr2(const TiXmlAttribute * attr, UIScaleMode * outModeA1, UIScaleMode * outModeA2,
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

static void uiScaleModeFromAttr4(const TiXmlAttribute * attr, UIScaleMode * outModeA1, UIScaleMode * outModeA2,
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

#define uiScaleModeFromAttr1(ATTR, WHAT) \
	uiScaleModeFromAttr1(ATTR, &m_##WHAT##ScaleMode, &m_Landscape##WHAT##ScaleMode, &has##WHAT##Scale)

#define uiScaleModeFromAttr2(ATTR, WHAT1, WHAT2) \
	uiScaleModeFromAttr2(ATTR, &m_##WHAT1##ScaleMode, &m_Landscape##WHAT1##ScaleMode, \
		&m_##WHAT2##ScaleMode, &m_Landscape##WHAT2##ScaleMode, &has##WHAT1##Scale, &has##WHAT2##Scale)

#define uiScaleModeFromAttr4(ATTR, WHAT1, WHAT2, WHAT3, WHAT4) \
	uiScaleModeFromAttr4(ATTR, &m_##WHAT1##ScaleMode, &m_Landscape##WHAT1##ScaleMode, \
		&m_##WHAT2##ScaleMode, &m_Landscape##WHAT2##ScaleMode, &m_##WHAT3##ScaleMode, \
		&m_Landscape##WHAT3##ScaleMode, &m_##WHAT4##ScaleMode, &m_Landscape##WHAT4##ScaleMode, \
		&has##WHAT1##Scale, &has##WHAT2##Scale, &has##WHAT3##Scale, &has##WHAT4##Scale)

static void uiAlignmentFromAttr(const TiXmlAttribute * attr, UIAlignment * outAlign1, UIAlignment * outAlign2)
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

void UIWidget::parse(const TiXmlElement * element)
{
	bool hasXScale = false, hasYScale = false, hasWidthScale = false, hasHeightScale = false;
	bool hasPos = false, hasSize = false;
	const TiXmlBase * xmlID = element;

	beforeParseAttributes(element);

	for (const TiXmlAttribute * attr = element->FirstAttribute(); attr; attr = attr->Next())
	{
		const std::string & name = attr->NameTStr();
		if (name == "id")
			m_ID = attr->ValueStr(), xmlID = attr;
		else if (name == "bgcolor")
			m_BackgroundColor = UIColor::fromAttr(attr);
		else if (name == "pos")
			uiFloatPairFromAttr(attr, &m_X, &m_Y, &m_LandscapeX, &m_LandscapeY), hasPos = true;
		else if (name == "size")
			uiFloatPairFromAttr(attr, &m_Width, &m_Height, &m_LandscapeWidth, &m_LandscapeHeight), hasSize = true;
		else if (name == "xscale")
			uiScaleModeFromAttr1(attr, X);
		else if (name == "yscale")
			uiScaleModeFromAttr1(attr, Y);
		else if (name == "wscale")
			uiScaleModeFromAttr1(attr, Width);
		else if (name == "hscale")
			uiScaleModeFromAttr1(attr, Height);
		else if (name == "xyscale")
			uiScaleModeFromAttr2(attr, X, Y);
		else if (name == "whscale")
			uiScaleModeFromAttr2(attr, Width, Height);
		else if (name == "xwscale")
			uiScaleModeFromAttr2(attr, X, Width);
		else if (name == "yhscale")
			uiScaleModeFromAttr2(attr, Y, Height);
		else if (name == "scale")
			uiScaleModeFromAttr4(attr, X, Y, Width, Height);
		else if (name == "align")
			uiAlignmentFromAttr(attr, &m_Alignment, &m_LandscapeAlignment);
		else if (!parseAttribute(attr))
			throw std::runtime_error(xmlError(attr, fmt() << "unknown attribute '" << name << "'."));
	}

	if (!hasPos)
		throw std::runtime_error(xmlMissingAttribute(element, "pos"));
	if (!hasSize)
		throw std::runtime_error(xmlMissingAttribute(element, "size"));

	if (!m_Layout->m_WidgetMap.insert(std::make_pair(m_ID, shared_from_this())).second)
		throw std::runtime_error(xmlError(xmlID, fmt() << "duplicate id '" << m_ID << "'."));

	afterParseAttributes(element);
}

void UIWidget::iosGenerateInitCode(const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << m_ID << ".backgroundColor = " << m_BackgroundColor.iosValue() << ";\n";
}

static std::string iosScaleFunc(UIScaleMode mode, bool horz)
{
	switch (mode)
	{
	case UIScaleDefault: return (horz ? "horzScale" : "vertScale");
	case UIScaleMin: return "std::min(horzScale, vertScale)";
	case UIScaleMax: return "std::max(horzScale, vertScale)";
	case UIScaleHorz: return "horzScale";
	case UIScaleVert: return "vertScale";
	case UIScaleAvg: return "((horzScale + vertScale) * 0.5f)";
	}

	assert(false);
	throw std::runtime_error("invalid scale mode.");
}

static void iosGenerateLayoutCode(const UIWidget * wd, const std::string & prefix, std::stringstream & ss,
	bool landscape)
{
	UIAlignment alignment = (!landscape ? wd->alignment() : wd->landscapeAlignment());

	float x = (!landscape ? wd->x() : wd->landscapeX());
	float y = (!landscape ? wd->y() : wd->landscapeY());
	float w = (!landscape ? wd->width() : wd->landscapeWidth());
	float h = (!landscape ? wd->height() : wd->landscapeHeight());

	std::string xMode = iosScaleFunc(!landscape ? wd->xScaleMode() : wd->landscapeXScaleMode(), true);
	std::string yMode = iosScaleFunc(!landscape ? wd->yScaleMode() : wd->landscapeYScaleMode(), false);
	std::string wMode = iosScaleFunc(!landscape ? wd->widthScaleMode() : wd->landscapeWidthScaleMode(), true);
	std::string hMode = iosScaleFunc(!landscape ? wd->heightScaleMode() : wd->landscapeHeightScaleMode(), false);

	ss << prefix << wd->id() << ".frame = iosLayoutRect<" << alignment << ">(" << x << ", " << y << ", " << w
		<< ", " << h << ", " << xMode << ", " << yMode << ", " << wMode << ", " << hMode
		<< ", horzScale, vertScale);\n";
}

void UIWidget::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << "if (landscape)\n";
	::iosGenerateLayoutCode(this, prefix + "\t", ss, true);
	ss << prefix << "else\n";
	::iosGenerateLayoutCode(this, prefix + "\t", ss, false);
}

void UIWidget::beforeParseAttributes(const TiXmlElement *)
{
}

bool UIWidget::parseAttribute(const TiXmlAttribute *)
{
	return false;
}

void UIWidget::afterParseAttributes(const TiXmlElement *)
{
}
