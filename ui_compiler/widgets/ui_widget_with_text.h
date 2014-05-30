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
#ifndef __c48137b06c14ea264a9986b4dfea3afd__
#define __c48137b06c14ea264a9986b4dfea3afd__

#include "ui_widget.h"
#include "../attributes/ui_font.h"

class UIWidgetWithText : public UIWidget
{
public:
	UIWidgetWithText(UILayout * layout, UIGroup * parentGroup, Kind kind);
	~UIWidgetWithText();

	inline const std::string & text() const { return m_Text; }
	inline const UIColor & textColor() const { return m_TextColor; }

	inline const UIFontPtr & font() const { return m_Font; }

	inline UIScaleMode fontScaleMode() const { return m_FontScaleMode; }
	inline UIScaleMode landscapeFontScaleMode() const { return m_LandscapeFontScaleMode; }

protected:
	bool parseAttribute(const TiXmlAttribute * attr) override;
	void afterParseAttributes(const TiXmlElement * element) override;

private:
	std::string m_Text;
	UIColor m_TextColor;
	UIFontPtr m_Font;
	UIScaleMode m_FontScaleMode;
	UIScaleMode m_LandscapeFontScaleMode;
	bool m_HasFontScaleMode;

	UIWidgetWithText(const UIWidgetWithText &) = delete;
	UIWidgetWithText & operator=(const UIWidgetWithText &) = delete;
};

#endif
