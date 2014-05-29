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

namespace
{
	struct WidgetInfo
	{
		UIWidget::Kind kind;
		UIWidgetPtr ipad;
		UIWidgetPtr iphone;
	};
}

typedef std::unordered_map<SourceFilePtr, UILayoutPtr> LayoutMap;
typedef std::unordered_map<std::string, WidgetInfo> WidgetInfos;

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

static WidgetInfos uiGetWidgetInfos(const std::initializer_list<UILayoutPtr> & layouts)
{
	WidgetInfos infos;

	size_t index = 0;
	for (const UILayoutPtr & layout : layouts)
	{
		++index;
		if (!layout)
			continue;

		for (auto it : layout->widgetMap())
		{
			const std::string & widgetID = it.first;
			const UIWidgetPtr & widget = it.second;

			WidgetInfo * infoPtr;

			auto jt = infos.find(widgetID);
			if (jt == infos.end())
			{
				WidgetInfo info;
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
			case 1: infoPtr->iphone = widget; break;
			case 2: infoPtr->ipad = widget; break;
			default: assert(false); throw std::runtime_error("internal error: invalid layout index.");
			}
		}
	}

	return infos;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// iOS

static void generateIOSCommonCode(const ProjectPtr & project)
{
	std::stringstream ss;
	ss << "#ifndef __fbaf7bcc319b33e8a39e16ddff3f11c7__\n";
	ss << "#define __fbaf7bcc319b33e8a39e16ddff3f11c7__\n";
	ss << '\n';
	ss << "namespace YIP\n";
	ss << "{\n";
	ss << "\ttemplate <unsigned char ALIGN> CGRect iosLayoutRect(float x, float y, float w, float h,\n";
	ss << "\t\tfloat xScale, float yScale, float wScale, float hScale, float horzScale, float vertScale)\n";
	ss << "\t{\n";
	ss << "\t\tfloat widgetX = x * xScale;\n";
	ss << "\t\tfloat widgetY = y * yScale;\n";
	ss << "\t\tfloat widgetW = w * wScale;\n";
	ss << "\t\tfloat widgetH = h * hScale;\n";
	ss << "\t\n";
	ss << "\t\tif ((ALIGN & " << UIAlignHorizontalMask << ") == " << UIAlignHCenter << ")\n";
	ss << "\t\t\twidgetX += (w * horzScale - widgetW) * 0.5f;\n";
	ss << "\t\telse if ((ALIGN & " << UIAlignHorizontalMask << ") == " << UIAlignRight << ")\n";
	ss << "\t\t\twidgetX += w * horzScale - widgetW;\n";
	ss << "\t\n";
	ss << "\t\tif ((ALIGN & " << UIAlignVerticalMask << ") == " << UIAlignVCenter << ")\n";
	ss << "\t\t\twidgetY += (h * vertScale - widgetH) * 0.5f;\n";
	ss << "\t\telse if ((ALIGN & " << UIAlignVerticalMask << ") == " << UIAlignBottom << ")\n";
	ss << "\t\t\twidgetY += h * vertScale - widgetH;\n";
	ss << "\t\n";
	ss << "\t\treturn CGRectMake(widgetX, widgetY, widgetW, widgetH);\n";
	ss << "\t}\n";
	ss << "}\n";
	ss << '\n';
	ss << "#endif\n";

	std::string targetPath = ".yip-ios-view-controllers/ios_layout.h";
	std::string generatedPath = project->yipDirectory()->writeFile(targetPath, ss.str());

	SourceFilePtr sourceFile = project->addSourceFile(targetPath, generatedPath);
	sourceFile->setIsGenerated(true);
	sourceFile->setPlatforms(Platform::iOS);
}

static void generateIOSViewController(LayoutMap & layouts, const ProjectPtr & project,
	const Project::IOSViewController & cntrl)
{
	std::string yipDir = project->yipDirectory()->path();

	SourceFilePtr sourceFileH;
	SourceFilePtr sourceFileM;

	std::string targetName = cntrl.name;
	std::string targetPathH = pathConcat(".yip-ios-view-controllers/yip-ios", targetName) + ".h";
	std::string targetPathM = pathConcat(".yip-ios-view-controllers/yip-ios", targetName) + ".mm";

	bool shouldProcessFile =
		(cntrl.iphone.get() &&
			project->yipDirectory()->shouldProcessFile(targetPathH, cntrl.iphone->path(), true) &&
			project->yipDirectory()->shouldProcessFile(targetPathM, cntrl.iphone->path(), true)) ||
		(cntrl.ipad.get() &&
			project->yipDirectory()->shouldProcessFile(targetPathH, cntrl.ipad->path(), true) &&
			project->yipDirectory()->shouldProcessFile(targetPathM, cntrl.ipad->path(), true));
	if (!shouldProcessFile)
	{
		sourceFileH = project->addSourceFile(targetPathH, pathConcat(yipDir, targetPathH));
		sourceFileM = project->addSourceFile(targetPathM, pathConcat(yipDir, targetPathM));
	}
	else
	{
		UILayoutPtr iphoneLayout = uiLoadLayout(layouts, cntrl.iphone);
		UILayoutPtr ipadLayout = uiLoadLayout(layouts, cntrl.ipad);

		WidgetInfos widgetInfos = uiGetWidgetInfos({
			// Don't change order of this items: it's important
			iphoneLayout,
			ipadLayout,
		});

		bool hasIPhone = iphoneLayout.get() != nullptr;
		bool hasIPad = ipadLayout.get() != nullptr;

		std::stringstream sh;
		sh << "#import <UIKit/UIKit.h>\n";
		sh << '\n';
		sh << "@interface " << cntrl.name << " : " << cntrl.parentClass << "\n";
		for (auto it : widgetInfos)
		{
			const UIWidgetPtr & widget = (it.second.iphone.get() ? it.second.iphone : it.second.ipad);
			sh << "@property (nonatomic, readonly, retain) " << widget->iosClassName()
				<< " * " << it.first << ";\n";
		}
		sh << "-(id)init;\n";
		sh << "-(void)dealloc;\n";
		sh << "@end\n";

		std::stringstream sm;
		sm << "#import \"" << targetName << ".h\"\n";
		sm << "#import \"../ios_layout.h\"\n";
		sm << '\n';
		sm << "@implementation " << cntrl.name << '\n';
		sm << '\n';
		for (auto it : widgetInfos)
			sm << "@synthesize " << it.first << ";\n";
		sm << '\n';
		sm << "-(id)init\n";
		sm << "{\n";
		sm << "\tself = [super init];\n";
		sm << "\tif (self)\n";
		sm << "\t{\n";
		if (hasIPhone)
		{
			sm << "\t\tif (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)\n";
			sm << "\t\t{\n";
			for (const UIWidgetPtr & widget : iphoneLayout->widgets())
			{
				widget->iosGenerateInitCode("\t\t\t", sm);
				sm << "\t\t\t[self.view addSubview:" << widget->id() << "];\n";
			}
			sm << "\t\t}\n";
		}
		if (hasIPad)
		{
			sm << "\t\tif (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)\n";
			sm << "\t\t{\n";
			for (const UIWidgetPtr & widget : ipadLayout->widgets())
			{
				widget->iosGenerateInitCode("\t\t\t", sm);
				sm << "\t\t\t[self.view addSubview:" << widget->id() << "];\n";
			}
			sm << "\t\t}\n";
		}
		sm << "\t}\n";
		sm << "\treturn self;\n";
		sm << "}\n";
		sm << '\n';
		sm << "-(void)dealloc\n";
		sm << "{\n";
		for (auto it : widgetInfos)
		{
			sm << "\t[" << it.first << " release];\n";
			sm << "\t" << it.first << " = nil;\n";
		}
		sm << "\t[super dealloc];\n";
		sm << "}\n";
		sm << '\n';
		sm << "-(void)viewWillLayoutSubviews\n";
		sm << "{\n";
		sm << "\t[super viewWillLayoutSubviews];\n";
		sm << '\n';
		sm << "\tCGRect frame = self.view.frame;\n";
		sm << "\tBOOL landscape = UIDeviceOrientationIsLandscape([UIDevice currentDevice].orientation);\n";
		sm << '\n';
		sm << "\t(void)frame;\n";
		sm << "\t(void)landscape;\n";
		sm << '\n';
		if (hasIPhone)
		{
			sm << "\tif (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)\n";
			sm << "\t{\n";
			sm << "\t\tconst float horzScale = frame.size.width / (landscape ? "
				<< iphoneLayout->landscapeWidth() << " : " << iphoneLayout->width() << ");\n";
			sm << "\t\tconst float vertScale = frame.size.height / (landscape ? "
				<< iphoneLayout->landscapeHeight() << " : " << iphoneLayout->height() << ");\n";
			sm << '\n';
			sm << "\t\t(void)horzScale;\n";
			sm << "\t\t(void)vertScale;\n";
			for (auto it : widgetInfos)
			{
				const UIWidgetPtr & w = it.second.iphone;
				if (!w.get())
					continue;
				sm << '\n';
				w->iosGenerateLayoutCode("\t\t", sm);
			}
			sm << "\t}\n";
		}
		if (hasIPad)
		{
			sm << "\tif (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)\n";
			sm << "\t{\n";
			sm << "\t\tconst float horzScale = frame.size.width / (landscape ? "
				<< ipadLayout->landscapeWidth() << " : " << ipadLayout->width() << ");\n";
			sm << "\t\tconst float vertScale = frame.size.height / (landscape ? "
				<< ipadLayout->landscapeHeight() << " : " << ipadLayout->height() << ");\n";
			sm << '\n';
			sm << "\t\t(void)horzScale;\n";
			sm << "\t\t(void)vertScale;\n";
			for (auto it : widgetInfos)
			{
				const UIWidgetPtr & w = it.second.ipad;
				if (!w.get())
					continue;
				sm << '\n';
				w->iosGenerateLayoutCode("\t\t", sm);
			}
			sm << "\t}\n";
		}
		sm << "}\n";
		sm << '\n';
		sm << "-(BOOL)shouldAutoRotate\n";
		sm << "{\n";
		sm << "\treturn YES;\n";
		sm << "}\n";
		sm << '\n';
		sm << "-(NSUInteger)supportedInterfaceOrientations\n";
		sm << "{\n";
		if (hasIPhone)
		{
			sm << "\tif (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)\n";
			sm << "\t\treturn 0";
			if (iphoneLayout->allowPortrait())
				sm << " | UIInterfaceOrientationMaskPortrait";
			if (iphoneLayout->allowLandscape())
				sm << " | UIInterfaceOrientationMaskLandscapeLeft | UIInterfaceOrientationMaskLandscapeRight";
			sm << ";\n";
		}
		if (hasIPad)
		{
			sm << "\tif (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)\n";
			sm << "\t\treturn 0";
			if (iphoneLayout->allowPortrait())
				sm << " | UIInterfaceOrientationMaskPortrait | UIInterfaceOrientationMaskPortraitUpsideDown";
			if (iphoneLayout->allowLandscape())
				sm << " | UIInterfaceOrientationMaskLandscapeLeft | UIInterfaceOrientationMaskLandscapeRight";
			sm << ";\n";
		}
		sm << "\treturn 0;\n";
		sm << "}\n";
		sm << '\n';
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
	generateIOSCommonCode(project);

	LayoutMap layouts;
	for (const Project::IOSViewController & cntrl : project->iosViewControllers())
		generateIOSViewController(layouts, project, cntrl);
}
