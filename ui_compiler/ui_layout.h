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
#ifndef __b869d7cae57c81e0e905d6c065d80352__
#define __b869d7cae57c81e0e905d6c065d80352__

#include "widgets/ui_widget.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class UILayout
{
public:
	UILayout();
	~UILayout();

	inline bool allowPortrait() const { return m_AllowPortrait; }
	inline bool allowLandscape() const { return m_AllowLandscape; }

	inline bool allowPortraitTablet() const { return m_AllowPortraitTablet; }
	inline bool allowLandscapeTablet() const { return m_AllowLandscapeTablet; }

	inline float width() const { return m_Width; }
	inline float height() const { return m_Height; }

	inline float landscapeWidth() const { return m_LandscapeWidth; }
	inline float landscapeHeight() const { return m_LandscapeHeight; }

	inline const std::vector<UIWidgetPtr> & widgets() const { return m_Widgets; }
	inline const std::unordered_map<std::string, UIWidgetPtr> & widgetMap() const { return m_WidgetMap; }
	UIWidgetPtr widgetForID(const std::string & id) const;

	void parse(const TiXmlDocument * doc);

private:
	size_t m_NextUniqueID;
	std::vector<UIWidgetPtr> m_Widgets;
	std::unordered_map<std::string, UIWidgetPtr> m_WidgetMap;
	float m_Width;
	float m_Height;
	float m_LandscapeWidth;
	float m_LandscapeHeight;
	bool m_AllowPortrait;
	bool m_AllowLandscape;
	bool m_AllowPortraitTablet;
	bool m_AllowLandscapeTablet;

	UILayout(const UILayout &) = delete;
	UILayout & operator=(const UILayout &) = delete;

	friend class UIWidget;
};

typedef std::shared_ptr<UILayout> UILayoutPtr;

#endif
