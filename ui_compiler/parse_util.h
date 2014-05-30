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
#ifndef __4b98ede777219a9429cb75279ed170ba__
#define __4b98ede777219a9429cb75279ed170ba__

#include "widgets/ui_widget.h"
#include "attributes/ui_scale_mode.h"
#include "attributes/ui_alignment.h"
#include "ui_layout.h"
#include "../3rdparty/tinyxml/tinyxml.h"

struct UIWidgetInfo
{
	UIWidget::Kind kind;
	UIWidgetPtr ipad;
	UIWidgetPtr iphone;
};

typedef std::unordered_map<std::string, UIWidgetInfo> UIWidgetInfos;
typedef std::unordered_map<SourceFilePtr, UILayoutPtr> UILayoutMap;

void uiFloatPairFromAttr(const TiXmlAttribute * attr, float * outX, float * outY,
	float * outLandscapeX, float * outLandscapeY);
void uiScaleModeFromAttr(const TiXmlAttribute * attr, UIScaleMode * outMode1, UIScaleMode * outMode2);
void uiScaleModeFromAttr1(const TiXmlAttribute * attr, UIScaleMode * outMode1, UIScaleMode * outMode2,
	bool * hasValue);
void uiScaleModeFromAttr2(const TiXmlAttribute * attr, UIScaleMode * outModeA1, UIScaleMode * outModeA2,
	UIScaleMode * outModeB1, UIScaleMode * outModeB2, bool * hasValueA, bool * hasValueB);
void uiScaleModeFromAttr4(const TiXmlAttribute * attr, UIScaleMode * outModeA1, UIScaleMode * outModeA2,
	UIScaleMode * outModeB1, UIScaleMode * outModeB2, UIScaleMode * outModeC1, UIScaleMode * outModeC2,
	UIScaleMode * outModeD1, UIScaleMode * outModeD2, bool * hasValueA, bool * hasValueB, bool * hasValueC,
	bool * hasValueD);
void uiAlignmentFromAttr(const TiXmlAttribute * attr, UIAlignment * outAlign1, UIAlignment * outAlign2);

#define uiScaleModeFromAttr1(ATTR, WHAT) \
	uiScaleModeFromAttr1(ATTR, &m_##WHAT##ScaleMode, &m_Landscape##WHAT##ScaleMode, &has##WHAT##Scale)

#define uiScaleModeFromAttr2(ATTR, WHAT1, WHAT2) \
	uiScaleModeFromAttr2(ATTR, &m_##WHAT1##ScaleMode, &m_Landscape##WHAT1##ScaleMode, \
		&m_##WHAT2##ScaleMode, &m_Landscape##WHAT2##ScaleMode, &has##WHAT1##Scale, &has##WHAT2##Scale)

#define uiScaleModeFromAttr4(ATTR, WHAT1, WHAT2, WHAT3, WHAT4) \
	uiScaleModeFromAttr4(ATTR, &m_##WHAT1##ScaleMode, &m_Landscape##WHAT1##ScaleMode, \
		&m_##WHAT2##ScaleMode, &m_Landscape##WHAT2##ScaleMode, &m_##WHAT3##ScaleMode, \
		&m_Landscape##WHAT3##ScaleMode, &m_##WHAT4##ScaleMode, &m_Landscape##WHAT4##ScaleMode, \
		&has##WHAT1##Scale, &has##WHAT2##Scale, &has##WHAT3##Scale, &has##WHAT4##Scale)

UILayoutPtr uiLoadLayout(UILayoutMap & layouts, const SourceFilePtr & sourceFile);
UIWidgetInfos uiGetWidgetInfos(const std::initializer_list<UILayoutPtr> & layouts);

#endif
