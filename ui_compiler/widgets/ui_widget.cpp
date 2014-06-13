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
#include "../ui_layout.h"
#include "ui_group.h"
#include "ui_button.h"
#include "ui_label.h"
#include "ui_image_view.h"
#include "ui_webview.h"
#include "ui_switch.h"
#include "ui_scroll_view.h"
#include "ui_spinner.h"
#include "ui_text_field.h"
#include "../parse_util.h"
#include "../../util/tinyxml-util/tinyxml-util.h"
#include "../../util/cxx-util/cxx-util/fmt.h"
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
	else if (className == "scrollview")
		return std::make_shared<UIScrollView>(layout, parentGroup);
	else if (className == "button")
		return std::make_shared<UIButton>(layout, parentGroup);
	else if (className == "label")
		return std::make_shared<UILabel>(layout, parentGroup);
	else if (className == "switch")
		return std::make_shared<UISwitch>(layout, parentGroup);
	else if (className == "image")
		return std::make_shared<UIImageView>(layout, parentGroup);
	else if (className == "textfield")
		return std::make_shared<UITextField>(layout, parentGroup);
	else if (className == "spinner")
		return std::make_shared<UISpinner>(layout, parentGroup);
	else if (className == "webview")
		return std::make_shared<UIWebView>(layout, parentGroup);

	throw std::runtime_error(fmt() << "invalid widget class '" << className << "'.");
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
