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

UITableView::Cell::Cell(bool isHeaderCell)
	: backgroundColor(UIColor::white),
	  androidParentClass("android.view.ViewGroup"),
	  iosParentClass(isHeaderCell ? "UITableViewHeaderFooterView" : "UITableViewCell"),
	  isHeader(isHeaderCell)
{
}

UITableView::UITableView(UILayout * layout, UIGroup * parentGroup)
	: UIGroup(layout, parentGroup, UIWidget::TableView),
	  m_RowHeight(0.0f),
	  m_LandscapeRowHeight(0.0f),
	  m_HasRowHeight(false)
{
	layout->m_HasTableViews = true;
}

UITableView::~UITableView()
{
}

bool UITableView::parseAttribute(const TiXmlAttribute * attr)
{
	if (attr->NameTStr() == "rowHeight")
	{
		uiFloatFromAttr(attr, &m_RowHeight, &m_LandscapeRowHeight);
		m_HasRowHeight = true;
		return true;
	}

	return UIGroup::parseAttribute(attr);
}

void UITableView::afterParseAttributes(const TiXmlElement * element)
{
	for (const TiXmlElement * child = element->FirstChildElement(); child; child = child->NextSiblingElement())
	{
		bool isHeaderCell = (child->ValueStr() == "header_cell");
		if (isHeaderCell || child->ValueStr() == "cell")
		{
			const TiXmlAttribute * classNameAttr = nullptr;
			bool hasSize = false;

			CellPtr cell = std::make_shared<Cell>(isHeaderCell);
			for (const TiXmlAttribute * attr = child->FirstAttribute(); attr; attr = attr->Next())
			{
				if (attr->NameTStr() == "ios:parentClass")
					cell->iosParentClass = attr->ValueStr();
				else if (attr->NameTStr() == "android:parentClass")
					cell->androidParentClass = attr->ValueStr();
				else if (attr->NameTStr() == "bgcolor")
					cell->backgroundColor = UIColor::fromAttr(attr);
				else if (attr->NameTStr() == "size")
					hasSize = true;
				else if (attr->NameTStr() == "className")
				{
					cell->className = attr->ValueStr();
					classNameAttr = attr;
				}
			}

			if (!classNameAttr)
				throw std::runtime_error(xmlMissingAttribute(child, "className"));
			if (!hasSize)
				throw std::runtime_error(xmlMissingAttribute(child, "size"));

			cell->layout = std::make_shared<UILayout>();
			cell->layout->parse(child, *layout());
			m_Cells.push_back(cell);
		}
		else
			throw std::runtime_error(xmlError(child, fmt() << "unexpected element '" << child->ValueStr() << "'."));
	}

	UIWidget::afterParseAttributes(element);
}
