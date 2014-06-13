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
#include "ui_layout.h"
#include "parse_util.h"
#include "../util/tinyxml-util/tinyxml-util.h"
#include "../util/cxx-util/cxx-util/fmt.h"

UILayout::UILayout()
	: m_NextUniqueID(1),
	  m_Width(0.0f),
	  m_Height(0.0f),
	  m_LandscapeWidth(0.0f),
	  m_LandscapeHeight(0.0f),
	  m_AllowPortrait(false),
	  m_AllowLandscape(false),
	  m_AllowPortraitTablet(false),
	  m_AllowLandscapeTablet(false)
{
}

UILayout::~UILayout()
{
}

UIWidgetPtr UILayout::widgetForID(const std::string & id) const
{
	auto it = m_WidgetMap.find(id);
	if (it != m_WidgetMap.end())
		return it->second;
	return UIWidgetPtr();
}

void UILayout::parse(const TiXmlDocument * doc)
{
	const TiXmlElement * element = xmlCheckRootElement(doc, "layout");

	for (const TiXmlAttribute * attr = element->FirstAttribute(); attr; attr = attr->Next())
	{
		const std::string & name = attr->NameTStr();
		if (name == "size")
			uiFloatPairFromAttr(attr, &m_Width, &m_Height, &m_LandscapeWidth, &m_LandscapeHeight);
		else if (name == "portrait")
			uiBoolPairFromAttr(attr, &m_AllowPortrait, &m_AllowPortraitTablet);
		else if (name == "landscape")
			uiBoolPairFromAttr(attr, &m_AllowLandscape, &m_AllowLandscapeTablet);
		else
			throw std::runtime_error(xmlError(attr, fmt() << "unknown attribute '" << name << "'."));
	}

	if (!m_AllowPortrait && !m_AllowLandscape)
		throw std::runtime_error(xmlError(element, "neither portrait, nor landscape orientation were enabled."));

	if (!m_AllowPortraitTablet && !m_AllowLandscapeTablet)
	{
		throw std::runtime_error(xmlError(element,
			"neither portrait, nor landscape orientation were enabled for tablets."));
	}

	for (const TiXmlElement * child = element->FirstChildElement(); child; child = child->NextSiblingElement())
	{
		if (child->ValueStr() == "string")
		{
			const TiXmlAttribute * id = nullptr, * text = nullptr;
			for (const TiXmlAttribute * attr = child->FirstAttribute(); attr; attr = attr->Next())
			{
				if (attr->NameTStr() == "id")
					id = attr;
				else if (attr->NameTStr() == "text")
					text = attr;
				else
				{
					throw std::runtime_error(xmlError(attr,
						fmt() << "unexpected attribute '" << attr->NameTStr() << "'."));
				}
			}

			if (!id)
				throw std::runtime_error(xmlMissingAttribute(child, "id"));
			if (!text)
				throw std::runtime_error(xmlMissingAttribute(child, "text"));

			if (id->ValueStr().empty())
				throw std::runtime_error(xmlInvalidAttributeValue(id));
			if (text->ValueStr().empty())
				throw std::runtime_error(xmlInvalidAttributeValue(text));

			if (!m_Strings.insert(std::make_pair(id->ValueStr(), text->ValueStr())).second)
				throw std::runtime_error(xmlError(id, fmt() << "duplicate string id '" << id->ValueStr() << "'."));

			continue;
		}

		UIWidgetPtr widget;
		try {
			widget = UIWidget::create(this, nullptr, child->ValueStr());
		} catch (const std::exception & e) {
			throw std::runtime_error(xmlError(child, e.what()));
		}

		widget->parse(child);
		m_Widgets.push_back(widget);
	}
}
