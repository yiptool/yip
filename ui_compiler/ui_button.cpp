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
#include "ui_button.h"
#include "../util/cxx_escape.h"
#include "../util/tinyxml-util/tinyxml-util.h"
#include "../util/cxx-util/cxx-util/fmt.h"

UIButton::UIButton(UILayout * layout, UIGroup * parentGroup)
	: UIWidget(layout, parentGroup, UIWidget::Button)
{
}

UIButton::~UIButton()
{
}

void iosChooseTranslation(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss,
	const std::string & text)
{
	if (project->translationFiles().size() == 0)
	{
		ss << "@\"";
		cxxEscape(ss, text);
		ss << '"';
		return;
	}

	ss << "YIP::iosChooseTranslation(@\"";
	cxxEscape(ss, text);
	ss << "\", @{\n";
	for (auto it : project->translationFiles())
	{
		ss << prefix << "\t@\"";
		cxxEscape(ss, it.first);
		ss << "\" : @\"";
		cxxEscape(ss, it.second->getTranslation(text));
		ss << "\",\n";
	}
	ss << prefix << "})";
}

void UIButton::iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << id() << " = [[UIButton buttonWithType:UIButtonTypeCustom] retain];\n";
	UIWidget::iosGenerateInitCode(project, prefix, ss);

	if (!m_Title.empty())
	{
		ss << prefix << '[' << id() << " setTitle:";
		iosChooseTranslation(project, prefix, ss, m_Title);
		ss << " forState:UIControlStateNormal];\n";
	}

	if (!m_Image.empty())
	{
		ss << prefix << '[' << id() << " setImage:[UIImage imageNamed:@\"";
		cxxEscape(ss, m_Image);
		ss << "\"] forState:UIControlStateNormal];\n";
	}
}

void UIButton::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::iosGenerateLayoutCode(prefix, ss);
}

bool UIButton::parseAttribute(const TiXmlAttribute * attr)
{
	if (attr->NameTStr() == "title")
	{
		m_Title = attr->ValueStr();
		return true;
	}
	else if (attr->NameTStr() == "image")
	{
		m_Image = attr->ValueStr();
		return true;
	}

	return false;
}
