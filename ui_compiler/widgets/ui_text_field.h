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
#ifndef __78c2b30fc8edc92db19541d158eb7c39__
#define __78c2b30fc8edc92db19541d158eb7c39__

#include "ui_widget_with_text.h"
#include "../attributes/ui_text_alignment.h"
#include <vector>

class UITextField : public UIWidgetWithText
{
public:
	UITextField(UILayout * layout, UIGroup * parentGroup);
	~UITextField();

	inline UITextAlignment textAlignment() const { return m_TextAlignment; }

	// In ios_generator.cpp
	void iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss,
		bool isViewController) override;
	void iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss) override;

	// In android_generator.cpp
	void androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
		std::stringstream & ss, std::map<std::string, std::string> & translations) override;
	void androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss) override;

protected:
	const char * iosDefaultClassName() const override { return "UITextField"; }
	const char * androidDefaultClassName() const override { return "android.widget.EditText"; }

	bool parseAttribute(const TiXmlAttribute * attr) override;

private:
	UITextAlignment m_TextAlignment;

	UITextField(const UITextField &) = delete;
	UITextField & operator=(const UITextField &) = delete;
};

#endif
