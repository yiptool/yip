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
#include "ui_switch.h"
#include "../../util/tinyxml-util/tinyxml-util.h"

UISwitch::UISwitch(UILayout * layout, UIGroup * parentGroup)
	: UIWidget(layout, parentGroup, UIWidget::Switch)
{
}

UISwitch::~UISwitch()
{
}

bool UISwitch::isCustom() const
{
	size_t len1 = m_KnobImage.length();
	size_t len2 = m_OnImage.length();
	size_t len3 = m_OffImage.length();
	return (len1 && len2 && len3);
}

bool UISwitch::parseAttribute(const TiXmlAttribute * attr)
{
	if (attr->NameTStr() == "onImage")
	{
		m_OnImage = attr->ValueStr();
		return true;
	}
	else if (attr->NameTStr() == "offImage")
	{
		m_OffImage = attr->ValueStr();
		return true;
	}
	else if (attr->NameTStr() == "knobImage")
	{
		m_KnobImage = attr->ValueStr();
		return true;
	}

	return UIWidget::parseAttribute(attr);
}

void UISwitch::afterParseAttributes(const TiXmlElement * element)
{
	size_t len1 = m_KnobImage.length();
	size_t len2 = m_OnImage.length();
	size_t len3 = m_OffImage.length();

	if (len1 || len2 || len3)
	{
		if (!len1 || !len2 || !len3)
			throw std::runtime_error(xmlError(element, "not all images were specified for the switch."));
	}

	UIWidget::afterParseAttributes(element);
}
