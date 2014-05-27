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
#include "../util/tinyxml-util/tinyxml-util.h"
#include "../util/cxx-util/cxx-util/fmt.h"

UIWidget::UIWidget()
	: m_BackgroundColor(UIColor::clear),
	  m_X(0.0f),
	  m_Y(0.0f),
	  m_Width(0.0f),
	  m_Height(0.0f),
	  m_XScaleMode(UIScaleDefault),
	  m_YScaleMode(UIScaleDefault),
	  m_WidthScaleMode(UIScaleDefault),
	  m_HeightScaleMode(UIScaleDefault)
{
}

UIWidget::~UIWidget()
{
}

void UIWidget::parse(const TiXmlElement * element)
{
	bool hasXScale = false, hasYScale = false, hasWidthScale = false, hasHeightScale = false, hasID = false;

	beforeParseAttributes(element);

	for (const TiXmlAttribute * attr = element->FirstAttribute(); attr; attr = attr->Next())
	{
		const std::string & name = attr->NameTStr();
		if (name == "id")
		{
			m_ID = attr->ValueStr();
			hasID = true;
		}
		else if (name == "bgcolor")
			m_BackgroundColor = UIColor::fromAttr(attr);
		else if (name == "pos")
		{
			std::vector<float> values;
			if (!xmlAttrToCommaSeparatedFloatList(attr, values) || values.size() != 2)
				throw std::runtime_error(xmlInvalidAttributeValue(attr));
			m_X = values[0];
			m_Y = values[1];
		}
		else if (name == "size")
		{
			std::vector<float> values;
			if (!xmlAttrToCommaSeparatedFloatList(attr, values) || values.size() != 2)
				throw std::runtime_error(xmlInvalidAttributeValue(attr));
			m_Width = values[0];
			m_Height = values[1];
		}
		else if (name == "xscale")
		{
			if (hasXScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the X coordinate."));
			m_XScaleMode = uiScaleModeFromAttr(attr);
			hasXScale = true;
		}
		else if (name == "yscale")
		{
			if (hasYScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the Y coordinate."));
			m_YScaleMode = uiScaleModeFromAttr(attr);
			hasYScale = true;
		}
		else if (name == "wscale")
		{
			if (hasWidthScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the width."));
			m_WidthScaleMode = uiScaleModeFromAttr(attr);
			hasWidthScale = true;
		}
		else if (name == "hscale")
		{
			if (hasHeightScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the height."));
			m_HeightScaleMode = uiScaleModeFromAttr(attr);
			hasHeightScale = true;
		}
		else if (name == "xwscale")
		{
			if (hasXScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the X coordinate."));
			if (hasWidthScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the width."));
			m_XScaleMode = uiScaleModeFromAttr(attr);
			m_WidthScaleMode = m_XScaleMode;
			hasXScale = true;
			hasWidthScale = true;
		}
		else if (name == "yhscale")
		{
			if (hasYScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the Y coordinate."));
			if (hasHeightScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the height."));
			m_YScaleMode = uiScaleModeFromAttr(attr);
			m_HeightScaleMode = m_YScaleMode;
			hasYScale = true;
			hasHeightScale = true;
		}
		else if (name == "xyscale")
		{
			if (hasXScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the X coordinate."));
			if (hasYScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the Y coordinate."));
			m_XScaleMode = uiScaleModeFromAttr(attr);
			m_YScaleMode = m_XScaleMode;
			hasXScale = true;
			hasYScale = true;
		}
		else if (name == "whscale")
		{
			if (hasWidthScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the width."));
			if (hasHeightScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the height."));
			m_WidthScaleMode = uiScaleModeFromAttr(attr);
			m_HeightScaleMode = m_WidthScaleMode;
			hasWidthScale = true;
			hasHeightScale = true;
		}
		else if (name == "scale")
		{
			if (hasXScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the X coordinate."));
			if (hasYScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the Y coordinate."));
			if (hasWidthScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the width."));
			if (hasHeightScale)
				throw std::runtime_error(xmlError(attr, "different scale mode for the height."));
			m_XScaleMode = uiScaleModeFromAttr(attr);
			m_YScaleMode = m_XScaleMode;
			m_WidthScaleMode = m_XScaleMode;
			m_HeightScaleMode = m_XScaleMode;
			hasXScale = true;
			hasYScale = true;
			hasWidthScale = true;
			hasHeightScale = true;
		}
		else
		{
			if (!parseAttribute(attr))
				throw std::runtime_error(xmlError(attr, fmt() << "unknown attribute '" << name << "'."));
		}
	}

	if (!hasID)
		throw std::runtime_error(xmlMissingAttribute(element, "id"));

	afterParseAttributes(element);
}

void beforeParseAttributes(const TiXmlElement *)
{
}

bool parseAttribute(const TiXmlAttribute *)
{
	return false;
}

void afterParseAttributes(const TiXmlElement *)
{
}
