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
#ifndef __df5c366bb5e5a0236358507a6b4b189e__
#define __df5c366bb5e5a0236358507a6b4b189e__

#include "ui_widget.h"
#include <vector>

class UIGroup : public UIWidget
{
public:
	UIGroup(UILayout * layout, UIGroup * parentGroup, Kind kind = UIWidget::Group);
	~UIGroup();

	// In ios_generator.cpp
	void iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss,
		bool isViewController) override;
	void iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss) override;

	// In android_generator.cpp
	void androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
		std::stringstream & ss, std::map<std::string, std::string> & translations) override;
	void androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss) override;

protected:
	const char * iosDefaultClassName() const override { return "UIView"; }
	const char * androidDefaultClassName() const override { return "DummyViewGroup"; }

	void afterParseAttributes(const TiXmlElement * element) override;

private:
	std::vector<UIWidgetPtr> m_Widgets;

	UIGroup(const UIGroup &) = delete;
	UIGroup & operator=(const UIGroup &) = delete;
};

#endif
