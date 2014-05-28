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
#ifndef __413ca771bbacd3af2f2cf3f63a13fff1__
#define __413ca771bbacd3af2f2cf3f63a13fff1__

#include "ui_scale_mode.h"
#include "ui_color.h"
#include "ui_alignment.h"
#include "../3rdparty/tinyxml/tinyxml.h"
#include <string>
#include <memory>

class UILayout;

class UIWidget;
typedef std::shared_ptr<UIWidget> UIWidgetPtr;

class UIWidget : public std::enable_shared_from_this<UIWidget>
{
public:
	enum Kind
	{
		Group = 0,
		Label,
		Image,
		Button
	};

	UIWidget(UILayout * layout, Kind kind);
	~UIWidget();

	static UIWidgetPtr create(UILayout * layout, const std::string & className);

	inline UILayout * layout() const { return m_Layout; }

	inline Kind kind() const { return m_Kind; }
	inline const std::string & id() const { return m_ID; }

	inline const UIColor & backgroundColor() const { return m_BackgroundColor; }

	inline float x() const { return m_X; }
	inline float y() const { return m_Y; }
	inline float width() const { return m_Width; }
	inline float height() const { return m_Height; }

	inline UIScaleMode xScaleMode() const { return m_XScaleMode; }
	inline UIScaleMode yScaleMode() const { return m_YScaleMode; }
	inline UIScaleMode widthScaleMode() const { return m_WidthScaleMode; }
	inline UIScaleMode heightScaleMode() const { return m_HeightScaleMode; }

	inline UIAlignment alignment() const { return m_Alignment; }

	void parse(const TiXmlElement * element);

	virtual void iosGenerateInitCode(std::stringstream & ss) = 0;

protected:
	virtual void beforeParseAttributes(const TiXmlElement * element);
	virtual bool parseAttribute(const TiXmlAttribute * attr);
	virtual void afterParseAttributes(const TiXmlElement * element);

private:
	UILayout * m_Layout;
	Kind m_Kind;
	std::string m_ID;
	UIColor m_BackgroundColor;
	float m_X;
	float m_Y;
	float m_Width;
	float m_Height;
	UIScaleMode m_XScaleMode;
	UIScaleMode m_YScaleMode;
	UIScaleMode m_WidthScaleMode;
	UIScaleMode m_HeightScaleMode;
	UIAlignment m_Alignment;

	UIWidget(const UIWidget &) = delete;
	UIWidget & operator=(const UIWidget &) = delete;
};

#endif
