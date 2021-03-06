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
#include "attributes/ui_image.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>

class TiXmlAttribute;

class UILayout;
typedef std::shared_ptr<UILayout> UILayoutPtr;

class UILayout : public std::enable_shared_from_this<UILayout>
{
public:
	struct Image
	{
		UIImagePtr m_ImagePtr;
		UIScaleMode m_WidthScaleMode = UIScaleDefault;
		UIScaleMode m_HeightScaleMode = UIScaleDefault;
		UIScaleMode m_LandscapeWidthScaleMode = UIScaleDefault;
		UIScaleMode m_LandscapeHeightScaleMode = UIScaleDefault;

		bool initFromXml(const TiXmlAttribute * attr, bool & hasWidthScale, bool & hasHeightScale);
	};

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

	inline bool hasTableViews() const { return m_HasTableViews; }

	inline const UIColor & backgroundColor() const { return m_BackgroundColor; }

	inline const std::vector<UIWidgetPtr> & widgets() const { return m_Widgets; }
	inline const std::unordered_map<std::string, UIWidgetPtr> & widgetMap() const { return m_WidgetMap; }
	UIWidgetPtr widgetForID(const std::string & id) const;

	inline const std::map<std::string, Image> & images() const { return m_Images; }
	inline const std::map<std::string, std::string> & strings() const { return m_Strings; }

	inline const std::vector<UILayoutPtr> & childLayouts() const { return m_ChildLayouts; }

	inline const std::set<std::string> & iosImports() const { return m_IOSImports; }

	void parse(const TiXmlDocument * doc);
	void parse(const TiXmlElement * element, UILayout & parentLayout);

private:
	size_t m_NextUniqueID;
	std::vector<UIWidgetPtr> m_Widgets;
	std::unordered_map<std::string, UIWidgetPtr> m_WidgetMap;
	std::map<std::string, Image> m_Images;
	std::map<std::string, std::string> m_Strings;
	std::set<std::string> m_IOSImports;
	std::vector<UILayoutPtr> m_ChildLayouts;
	float m_Width;
	float m_Height;
	float m_LandscapeWidth;
	float m_LandscapeHeight;
	bool m_AllowPortrait;
	bool m_AllowLandscape;
	bool m_AllowPortraitTablet;
	bool m_AllowLandscapeTablet;
	bool m_HasTableViews;
	UIColor m_BackgroundColor;

	void parseWidgetList(const TiXmlElement * element, bool allowStrings);

	UILayout(const UILayout &) = delete;
	UILayout & operator=(const UILayout &) = delete;

	friend class UIWidget;
	friend class UITableView;
};

#endif
