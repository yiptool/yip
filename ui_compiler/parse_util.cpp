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
#include "parse_util.h"
#include "widgets/ui_table_view.h"
#include "../util/tinyxml-util/tinyxml-util.h"
#include "../util/cxx-util/cxx-util/fmt.h"
#include "../util/cxx-util/cxx-util/explode.h"
#include "../util/cxx-util/cxx-util/trim.h"
#include "../util/strtod/strtod.h"

#undef uiScaleModeFromAttr1
#undef uiScaleModeFromAttr2
#undef uiScaleModeFromAttr4

static bool strToBool(const std::string & str, bool * out)
{
	if (str == "true" || str == "yes")
	{
		*out = true;
		return true;
	}
	else if (str == "false" || str == "no")
	{
		*out = false;
		return true;
	}
	return false;
}

void uiBoolPairFromAttr(const TiXmlAttribute * attr, bool * out1, bool * out2)
{
	std::vector<std::string> values = explode(attr->ValueStr(), '/');

	if (values.size() < 1 || values.size() > 2)
		throw std::runtime_error(xmlInvalidAttributeValue(attr));

	if (!strToBool(values[0], out1))
		throw std::runtime_error(xmlInvalidAttributeValue(attr));

	if (!strToBool(values.size() > 1 ? values[1] : values[0], out2))
		throw std::runtime_error(xmlInvalidAttributeValue(attr));
}

void uiFloatFromAttr(const TiXmlAttribute * attr, float * out1, float * out2)
{
	std::vector<std::string> lists = explode(attr->ValueStr(), '/');
	if (lists.size() < 1 || lists.size() > 2)
		throw std::runtime_error(xmlInvalidAttributeValue(attr));

	const std::string & val1 = lists[0];
	const std::string & val2 = (lists.size() > 1 ? lists[1] : lists[0]);

	if (!strToFloat(trim(val1), *out1) || !strToFloat(trim(val2), *out2))
		throw std::runtime_error(xmlInvalidAttributeValue(attr));
}

void uiFloatPairFromAttr(const TiXmlAttribute * attr, float * outX, float * outY,
	float * outLandscapeX, float * outLandscapeY)
{
	std::vector<std::string> lists = explode(attr->ValueStr(), '/');
	if (lists.size() < 1 || lists.size() > 2)
		throw std::runtime_error(xmlInvalidAttributeValue(attr));

	std::vector<std::string> p = explode(lists[0], ',');
	std::vector<std::string> l = explode(lists.size() > 1 ? lists[1] : lists[0], ',');
	if (p.size() != 2 || l.size() != 2)
		throw std::runtime_error(xmlInvalidAttributeValue(attr));

	if (!strToFloat(trim(p[0]), *outX) || !strToFloat(trim(p[1]), *outY) ||
			!strToFloat(trim(l[0]), *outLandscapeX) || !strToFloat(trim(l[1]), *outLandscapeY))
		throw std::runtime_error(xmlInvalidAttributeValue(attr));
}

void uiScaleModeFromAttr(const TiXmlAttribute * attr, UIScaleMode * outMode1, UIScaleMode * outMode2)
{
	std::vector<std::string> list = explode(attr->ValueStr(), '/');
	if (list.size() < 1 || list.size() > 2)
		throw std::runtime_error(xmlInvalidAttributeValue(attr));

	try
	{
		*outMode1 = uiScaleModeFromString(list[0]);
		*outMode2 = uiScaleModeFromString(list.size() > 1 ? list[1] : list[0]);
	}
	catch (const std::exception & e)
	{
		throw std::runtime_error(xmlError(attr, e.what()));
	}
}

void uiScaleModeFromAttr1(const TiXmlAttribute * attr, UIScaleMode * outMode1, UIScaleMode * outMode2,
	bool * hasValue)
{
	if (*hasValue)
		throw std::runtime_error(xmlError(attr, "multiple scale modes for the same attribute."));
	uiScaleModeFromAttr(attr, outMode1, outMode2);
	*hasValue = true;
}

void uiScaleModeFromAttr2(const TiXmlAttribute * attr, UIScaleMode * outModeA1, UIScaleMode * outModeA2,
	UIScaleMode * outModeB1, UIScaleMode * outModeB2, bool * hasValueA, bool * hasValueB)
{
	if (*hasValueA || *hasValueB)
		throw std::runtime_error(xmlError(attr, "multiple scale modes for the same attribute."));

	uiScaleModeFromAttr(attr, outModeA1, outModeA2);
	*outModeB1 = *outModeA1;
	*outModeB2 = *outModeA2;

	*hasValueA = true;
	*hasValueB = true;
}

void uiScaleModeFromAttr4(const TiXmlAttribute * attr, UIScaleMode * outModeA1, UIScaleMode * outModeA2,
	UIScaleMode * outModeB1, UIScaleMode * outModeB2, UIScaleMode * outModeC1, UIScaleMode * outModeC2,
	UIScaleMode * outModeD1, UIScaleMode * outModeD2, bool * hasValueA, bool * hasValueB, bool * hasValueC,
	bool * hasValueD)
{
	if (*hasValueA || *hasValueB || *hasValueC || *hasValueD)
		throw std::runtime_error(xmlError(attr, "multiple scale modes for the same attribute."));

	uiScaleModeFromAttr(attr, outModeA1, outModeA2);
	*outModeB1 = *outModeA1;
	*outModeB2 = *outModeA2;
	*outModeC1 = *outModeA1;
	*outModeC2 = *outModeA2;
	*outModeD1 = *outModeA1;
	*outModeD2 = *outModeA2;

	*hasValueA = true;
	*hasValueB = true;
	*hasValueC = true;
	*hasValueD = true;
}

void uiAlignmentFromAttr(const TiXmlAttribute * attr, UIAlignment * outAlign1, UIAlignment * outAlign2)
{
	std::vector<std::string> list = explode(attr->ValueStr(), '/');
	if (list.size() < 1 || list.size() > 2)
		throw std::runtime_error(xmlInvalidAttributeValue(attr));

	try
	{
		*outAlign1 = uiAlignmentFromString(list[0]);
		*outAlign2 = uiAlignmentFromString(list.size() > 1 ? list[1] : list[0]);
	}
	catch (const std::exception & e)
	{
		throw std::runtime_error(xmlError(attr, e.what()));
	}
}

UILayoutPtr uiLoadLayout(UILayoutMap & layouts, const SourceFilePtr & sourceFile)
{
	if (!sourceFile.get())
		return UILayoutPtr();

	auto it = layouts.find(sourceFile);
	if (it != layouts.end())
		return it->second;

	std::cout << "parsing " << sourceFile->name() << std::endl;

	TiXmlDocument doc(sourceFile->path());
	if (!doc.LoadFile())
	{
		std::stringstream ss;
		ss << "error in '" << doc.ValueStr() << "' at row " << doc.ErrorRow() << ", column " << doc.ErrorCol()
			<< ": " << doc.ErrorDesc();
		throw std::runtime_error(ss.str());
	}

	UILayoutPtr layout = std::make_shared<UILayout>();
	layout->parse(&doc);

	layouts.insert(std::make_pair(sourceFile, layout));

	return layout;
}

static void uiGetWidgetInfos(size_t index, const UILayoutPtr & layout, UIWidgetInfos & infos, bool android)
{
	if (!layout)
		return;

	for (auto it : layout->widgetMap())
	{
		const std::string & widgetID = it.first;
		const UIWidgetPtr & widget = it.second;

		UIWidgetInfo * infoPtr;

		auto jt = infos.find(widgetID);
		if (jt == infos.end())
		{
			UIWidgetInfo info;
			info.kind = widget->kind();
			infoPtr = &infos.insert(std::make_pair(widgetID, info)).first->second;
		}
		else
		{
			infoPtr = &jt->second;
			if (jt->second.kind != widget->kind())
			{
				throw std::runtime_error(fmt() << "id '"
					<< widgetID << "' corresponds to different widgets in different layout files.");
			}
		}

		switch (index)
		{
		case 1: (android ? infoPtr->phone = widget : infoPtr->iphone = widget); break;
		case 2: (android ? infoPtr->tablet7 = widget : infoPtr->ipad = widget); break;
		case 3: infoPtr->tablet10 = widget; break;
		default: assert(false); throw std::runtime_error("internal error: invalid layout index.");
		}
	}
}

UIWidgetInfos uiGetWidgetInfos(const std::initializer_list<UILayoutPtr> & layouts, bool android)
{
	UIWidgetInfos infos;

	size_t index = 0;
	for (const UILayoutPtr & layout : layouts)
	{
		++index;
		uiGetWidgetInfos(index, layout, infos, android);
	}

	return infos;
}

static void uiGetTableCellClasses(size_t index, const UILayoutPtr & layout, UITableCellInfos & classes,
	std::set<std::string> & classNames, bool android)
{
	for (const auto & widget : layout->widgets())
	{
		if (widget->kind() != UIWidget::TableView)
			continue;

		for (const auto & cell : static_cast<UITableView *>(widget.get())->cells())
		{
			const std::string & className = cell->className;
			if (!classNames.insert(className).second)
				throw std::runtime_error(fmt() << "duplicate cell class name: '" << className << "'.");

			auto & cellInfo = classes[className];
			cellInfo.cell = cell;
			uiGetWidgetInfos(index, cell->layout, cellInfo.widgetInfos, android);

			uiGetTableCellClasses(index, cell->layout, classes, classNames, android);
		}
	}
}

UITableCellInfos uiGetTableCellClasses(const std::initializer_list<UILayoutPtr> & layouts, bool android)
{
	UITableCellInfos classes;
	size_t index = 0;

	for (const UILayoutPtr & layout : layouts)
	{
		++index;
		std::set<std::string> layoutClasses;
		uiGetTableCellClasses(index, layout, classes, layoutClasses, android);
	}

	return classes;
}
