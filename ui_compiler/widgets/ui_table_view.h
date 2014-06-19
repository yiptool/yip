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
#ifndef __37973fb7b4f766d77f24e000f6143305__
#define __37973fb7b4f766d77f24e000f6143305__

#include "ui_group.h"
#include "../ui_layout.h"
#include <vector>
#include <memory>

class UITableView : public UIGroup
{
public:
	struct Cell
	{
		UILayoutPtr layout;
		UIColor backgroundColor;
		std::string className;
		std::string androidParentClass;
		std::string iosParentClass;
		bool isHeader;

		Cell(bool isHeaderCell);
	};

	typedef std::shared_ptr<Cell> CellPtr;

	UITableView(UILayout * layout, UIGroup * parentGroup);
	~UITableView();

	inline float rowHeight() const { return m_RowHeight; }
	inline float landscapeRowHeight() const { return m_LandscapeRowHeight; }
	inline bool hasRowHeight() const { return m_HasRowHeight; }

	inline const std::vector<CellPtr> & cells() const { return m_Cells; }

	// In ios_generator.cpp
	void iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss,
		bool isViewController) override;
	void iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss) override;

	// In android_generator.cpp
	void androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
		std::stringstream & ss, std::map<std::string, std::string> & translations) override;
	void androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss) override;

protected:
	const char * iosDefaultClassName() const override { return "UITableView"; }
	const char * androidDefaultClassName() const override { return "android.widget.ListView"; }

	bool parseAttribute(const TiXmlAttribute * attr) override;
	void afterParseAttributes(const TiXmlElement * element) override;

private:
	std::vector<CellPtr> m_Cells;
	float m_RowHeight;
	float m_LandscapeRowHeight;
	bool m_HasRowHeight;

	UITableView(const UITableView &) = delete;
	UITableView & operator=(const UITableView &) = delete;
};

#endif
