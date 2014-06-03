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
#ifndef __854a5b920b3b3fa177f36ee0e5938aff__
#define __854a5b920b3b3fa177f36ee0e5938aff__

#include "ui_widget.h"
#include <vector>

class UISwitch : public UIWidget
{
public:
	UISwitch(UILayout * layout, UIGroup * parentGroup);
	~UISwitch();

	bool isCustom() const;

	// In ios_generator.cpp
	const char * iosClassName() const override { return isCustom() ? "NZSwitchControl" : "UISwitch"; }
	void iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss) override;
	void iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss) override;

	// In android_generator.cpp
	const char * androidClassName() const override { return "Switch"; }

protected:
	bool parseAttribute(const TiXmlAttribute * attr) override;
	void afterParseAttributes(const TiXmlElement * element) override;

private:
	std::string m_KnobImage;
	std::string m_OnImage;
	std::string m_OffImage;

	UISwitch(const UISwitch &) = delete;
	UISwitch & operator=(const UISwitch &) = delete;
};

#endif
