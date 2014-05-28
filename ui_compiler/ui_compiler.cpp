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
#include "ui_compiler.h"
#include "ui_layout.h"
#include "../3rdparty/tinyxml/tinyxml.h"
#include "../util/tinyxml-util/tinyxml-util.h"
#include "../util/cxx-util/cxx-util/fmt.h"
#include "../util/path.h"
#include "../util/sha1.h"
#include <cassert>
#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>

typedef std::unordered_map<SourceFilePtr, UILayoutPtr> LayoutMap;
typedef std::unordered_map<std::string, UIWidget::Kind> WidgetKinds;

static UILayoutPtr uiLoadLayout(LayoutMap & layouts, const SourceFilePtr & sourceFile)
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

static WidgetKinds uiGetWidgetKinds(const std::initializer_list<UILayoutPtr> & layouts)
{
	WidgetKinds kinds;

	for (const UILayoutPtr & layout : layouts)
	{
		if (!layout)
			continue;

		for (auto it : layout->widgetMap())
		{
			const std::string & widgetID = it.first;
			const UIWidgetPtr & widget = it.second;

			auto jt = kinds.find(widgetID);
			if (jt == kinds.end())
				kinds.insert(std::make_pair(widgetID, widget->kind()));
			else if (jt->second != widget->kind())
			{
				throw std::runtime_error(fmt() << "id '"
					<< widgetID << "' corresponds to different widgets in different layout files.");
			}
		}
	}

	return kinds;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// iOS

static std::string iosClassForWidget(UIWidget::Kind kind)
{
	switch (kind)
	{
	case UIWidget::Group: return "UIView";
	case UIWidget::Label: return "UILabel";
	case UIWidget::Image: return "UIImage";
	case UIWidget::Button: return "UIButton";
	}

	assert(false);
	throw std::runtime_error("invalid widget class.");
}

static std::string iosInitForWidget(UIWidget::Kind kind)
{
	switch (kind)
	{
	case UIWidget::Group: return "[[UIView alloc] initWithFrame:CGRectZero]";
	case UIWidget::Label: return "[[UILabel alloc] initWithFrame:CGRectZero]";
	case UIWidget::Image: return "[[UIImageView alloc] initWithImage:nil]";
	case UIWidget::Button: return "[[UIButton buttonWithType:UIButtonTypeCustom] retain]";
	}

	assert(false);
	throw std::runtime_error("invalid widget class.");
}

static void generateIOSViewController(LayoutMap & layouts, const ProjectPtr & project,
	const Project::IOSViewController & cntrl)
{
	std::string yipDir = project->yipDirectory()->path();

	SourceFilePtr sourceFileH;
	SourceFilePtr sourceFileM;

	std::string targetName = cntrl.name;
	std::string targetPathH = pathConcat(".yip-ios-view-controllers/yip-ios", targetName) + ".h";
	std::string targetPathM = pathConcat(".yip-ios-view-controllers/yip-ios", targetName) + ".m";

	bool shouldProcessFile =
		(cntrl.iphonePortrait.get() &&
			project->yipDirectory()->shouldProcessFile(targetPathH, cntrl.iphonePortrait->path()) &&
			project->yipDirectory()->shouldProcessFile(targetPathM, cntrl.iphonePortrait->path())) ||
		(cntrl.iphoneLandscape.get() &&
			project->yipDirectory()->shouldProcessFile(targetPathH, cntrl.iphoneLandscape->path()) &&
			project->yipDirectory()->shouldProcessFile(targetPathM, cntrl.iphoneLandscape->path())) ||
		(cntrl.ipadPortrait.get() &&
			project->yipDirectory()->shouldProcessFile(targetPathH, cntrl.ipadPortrait->path()) &&
			project->yipDirectory()->shouldProcessFile(targetPathM, cntrl.ipadPortrait->path())) ||
		(cntrl.ipadLandscape.get() &&
			project->yipDirectory()->shouldProcessFile(targetPathH, cntrl.ipadLandscape->path()) &&
			project->yipDirectory()->shouldProcessFile(targetPathM, cntrl.ipadLandscape->path()));
	if (!shouldProcessFile)
	{
		sourceFileH = project->addSourceFile(targetPathH, pathConcat(yipDir, targetPathH));
		sourceFileM = project->addSourceFile(targetPathM, pathConcat(yipDir, targetPathM));
	}
	else
	{
		UILayoutPtr iphonePortrait = uiLoadLayout(layouts, cntrl.iphonePortrait);
		UILayoutPtr iphoneLandscape = uiLoadLayout(layouts, cntrl.iphoneLandscape);
		UILayoutPtr ipadPortrait = uiLoadLayout(layouts, cntrl.ipadPortrait);
		UILayoutPtr ipadLandscape = uiLoadLayout(layouts, cntrl.ipadLandscape);

		WidgetKinds widgetKinds = uiGetWidgetKinds({
			iphonePortrait,
			iphoneLandscape,
			ipadPortrait,
			ipadLandscape
		});

		std::stringstream sh;
		sh << "#import <UIKit/UIKit.h>\n";
		sh << "@interface " << cntrl.name << " : " << cntrl.parentClass << "\n";
		for (auto it : widgetKinds)
		{
			sh << "@property (nonatomic, readonly, retain) " << iosClassForWidget(it.second)
				<< ' ' << it.first << ";\n";
		}
		sh << "-(id)init;\n";
		sh << "-(void)dealloc;\n";
		sh << "@end\n";

		std::stringstream sm;
		sm << "#import \"" << targetName << ".h\"\n";
		sm << "@implementation " << cntrl.name << '\n';
		for (auto it : widgetKinds)
			sm << "@synthesize " << it.first << ";\n";
		sm << "-(id)init\n";
		sm << "{\n";
		sm << "\tself = [super init];\n";
		sm << "\tif (self)\n";
		sm << "\t{\n";
		for (auto it : widgetKinds)
			sm << "\t\t" << it.first << " = " << iosInitForWidget(it.second) << ";\n";
		sm << "\t}\n";
		sm << "\treturn self;\n";
		sm << "}\n";
		sm << "-(void)dealloc\n";
		sm << "{\n";
		for (auto it : widgetKinds)
		{
			sm << "\t[" << it.first << " release];\n";
			sm << "\t" << it.first << " = nil;\n";
		}
		sm << "\t[super dealloc];\n";
		sm << "}\n";
		sm << "@end\n";

		std::string generatedPathH = project->yipDirectory()->writeFile(targetPathH, sh.str());
		sourceFileH = project->addSourceFile(targetPathH, generatedPathH);

		std::string generatedPathM = project->yipDirectory()->writeFile(targetPathM, sm.str());
		sourceFileM = project->addSourceFile(targetPathM, generatedPathM);
	}

	sourceFileH->setIsGenerated(true);
	sourceFileH->setPlatforms(Platform::iOS);

	sourceFileM->setIsGenerated(true);
	sourceFileM->setPlatforms(Platform::iOS);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void compileUI(const ProjectPtr & project)
{
	LayoutMap layouts;
	for (const Project::IOSViewController & cntrl : project->iosViewControllers())
		generateIOSViewController(layouts, project, cntrl);
}
