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
#include "ui_table_view.h"
#include "../parse_util.h"
#include "../../util/tinyxml-util/tinyxml-util.h"
#include "../../util/cxx-util/cxx-util/fmt.h"
#include <stdexcept>

UITableView::Cell::Cell()
	: height(0.0f),
	  landscapeHeight(0.0f),
	  androidParentClass("android.view.ViewGroup"),
	  iosParentClass("UITableViewCell")
{
}

UITableView::UITableView(UILayout * layout, UIGroup * parentGroup)
	: UIGroup(layout, parentGroup, UIWidget::TableView)
{
}

UITableView::~UITableView()
{
}

void UITableView::afterParseAttributes(const TiXmlElement * element)
{
	for (const TiXmlElement * child = element->FirstChildElement(); child; child = child->NextSiblingElement())
	{
		if (child->ValueStr() == "cell")
		{
			const TiXmlAttribute * classNameAttr = nullptr;
			CellPtr cell = std::make_shared<Cell>();
			bool hasHeight = false;

			for (const TiXmlAttribute * attr = child->FirstAttribute(); attr; attr = attr->Next())
			{
				if (attr->NameTStr() == "ios:parentClass")
					cell->iosParentClass = attr->ValueStr();
				else if (attr->NameTStr() == "android:parentClass")
					cell->androidParentClass = attr->ValueStr();
				else if (attr->NameTStr() == "className")
				{
					cell->className = attr->ValueStr();
					classNameAttr = attr;
				}
				else if (attr->NameTStr() == "height")
				{
					uiFloatFromAttr(attr, &cell->height, &cell->landscapeHeight);
					hasHeight = true;
				}
			}

			if (!classNameAttr)
				throw std::runtime_error(xmlMissingAttribute(child, "className"));
			if (!hasHeight)
				throw std::runtime_error(xmlMissingAttribute(child, "height"));

			cell->layout = std::make_shared<UILayout>();
			cell->layout->parse(child, *layout());
		}
		else
			throw std::runtime_error(xmlError(child, fmt() << "unexpected element '" << child->ValueStr() << "'."));
	}

	UIWidget::afterParseAttributes(element);
}
