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
#include "ios_generator.h"
#include "parse_util.h"
#include "widgets/ui_widget.h"
#include "widgets/ui_group.h"
#include "widgets/ui_button.h"
#include "widgets/ui_label.h"
#include "widgets/ui_image.h"
#include "../util/cxx-util/cxx-util/fmt.h"
#include "../util/path-util/path-util.h"
#include "../util/cxx_escape.h"
#include <cassert>
#include <stdexcept>

std::string iosScaleFunc(UIScaleMode mode, bool horz)
{
	switch (mode)
	{
	case UIScaleDefault: return (horz ? "horzScale" : "vertScale");
	case UIScaleMin: return "MIN(horzScale, vertScale)";
	case UIScaleMax: return "MAX(horzScale, vertScale)";
	case UIScaleHorz: return "horzScale";
	case UIScaleVert: return "vertScale";
	case UIScaleAvg: return "((horzScale + vertScale) * 0.5f)";
	}

	assert(false);
	throw std::runtime_error("invalid scale mode.");
}

void iosChooseTranslation(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss,
	const std::string & text)
{
	if (project->translationFiles().size() == 0)
	{
		ss << "@\"";
		cxxEscape(ss, text);
		ss << '"';
		return;
	}

	ss << "YIP::iosChooseTranslation(@\"";
	cxxEscape(ss, text);
	ss << "\", @{\n";
	for (auto it : project->translationFiles())
	{
		ss << prefix << "\t@\"";
		cxxEscape(ss, it.first);
		ss << "\" : @\"";
		cxxEscape(ss, it.second->getTranslation(text));
		ss << "\",\n";
	}
	ss << prefix << "})";
}

void iosGetFont(std::stringstream & ss, const UIFontPtr & font, UIScaleMode scaleMode,
	UIScaleMode landscapeScaleMode)
{
	ss << "YIP::iosGetFont(@\"";
	cxxEscape(ss, font->family);
	ss << "\", (landscape ? " << font->landscapeSize << " * " << iosScaleFunc(landscapeScaleMode, false);
	ss << " : " << font->size << " * " << iosScaleFunc(scaleMode, false) << "))";
}

void iosGenerateLayoutCode(const UIWidget * wd, const std::string & prefix, std::stringstream & ss, bool landscape)
{
	UIAlignment alignment = (!landscape ? wd->alignment() : wd->landscapeAlignment());

	float x = (!landscape ? wd->x() : wd->landscapeX());
	float y = (!landscape ? wd->y() : wd->landscapeY());
	float w = (!landscape ? wd->width() : wd->landscapeWidth());
	float h = (!landscape ? wd->height() : wd->landscapeHeight());

	std::string xMode = iosScaleFunc(!landscape ? wd->xScaleMode() : wd->landscapeXScaleMode(), true);
	std::string yMode = iosScaleFunc(!landscape ? wd->yScaleMode() : wd->landscapeYScaleMode(), false);
	std::string wMode = iosScaleFunc(!landscape ? wd->widthScaleMode() : wd->landscapeWidthScaleMode(), true);
	std::string hMode = iosScaleFunc(!landscape ? wd->heightScaleMode() : wd->landscapeHeightScaleMode(), false);

	ss << prefix << wd->id() << ".frame = YIP::iosLayoutRect<" << alignment << ">(" << x << ", " << y << ", "
		<< w << ", " << h << ", " << xMode << ", " << yMode << ", " << wMode << ", " << hMode
		<< ", horzScale, vertScale);\n";
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIColor

std::string UIColor::iosValue() const
{
	for (auto it : UIColor::names)
	{
		if (*this == it.second)
			return fmt() << "[UIColor " << it.first << "Color]";
	}

	std::stringstream ss;
	ss << "[UIColor colorWithRed:" << r / 255.0f << " green:" << g / 255.0f << " blue:" << b / 255.0f
		<< " alpha:" << a << "]";

	return ss.str();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIWidget

void UIWidget::iosGenerateInitCode(const ProjectPtr &, const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << m_ID << ".backgroundColor = " << m_BackgroundColor.iosValue() << ";\n";
}

void UIWidget::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << "if (landscape)\n";
	::iosGenerateLayoutCode(this, prefix + "\t", ss, true);
	ss << prefix << "else\n";
	::iosGenerateLayoutCode(this, prefix + "\t", ss, false);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIGroup

void UIGroup::iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << id() << " = [[UIView alloc] initWithFrame:CGRectZero];\n";
	UIWidget::iosGenerateInitCode(project, prefix, ss);

	for (const UIWidgetPtr & widget : m_Widgets)
	{
		widget->iosGenerateInitCode(project, prefix, ss);
		ss << prefix << "[" << id() << " addSubview:" << widget->id() << "];\n";
	}
}

void UIGroup::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::iosGenerateLayoutCode(prefix, ss);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UILabel

void UILabel::iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << id() << " = [[UILabel alloc] initWithFrame:CGRectZero];\n";
	UIWidget::iosGenerateInitCode(project, prefix, ss);
}

void UILabel::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::iosGenerateLayoutCode(prefix, ss);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIImage

void UIImage::iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << id() << " = [[UIImageView alloc] initWithImage:nil];\n";
	UIWidget::iosGenerateInitCode(project, prefix, ss);
}

void UIImage::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::iosGenerateLayoutCode(prefix, ss);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIButton

void UIButton::iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << id() << " = [[UIButton buttonWithType:UIButtonTypeCustom] retain];\n";
	UIWidget::iosGenerateInitCode(project, prefix, ss);

	if (!text().empty())
	{
		ss << prefix << '[' << id() << " setTitle:";
		iosChooseTranslation(project, prefix, ss, text());
		ss << " forState:UIControlStateNormal];\n";
	}

	if (!m_Image.empty())
	{
		ss << prefix << '[' << id() << " setImage:[UIImage imageNamed:@\"";
		cxxEscape(ss, m_Image);
		ss << "\"] forState:UIControlStateNormal];\n";
	}
}

void UIButton::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::iosGenerateLayoutCode(prefix, ss);

	if (font().get())
	{
		ss << prefix << id() << ".titleLabel.font = ";
		iosGetFont(ss, font(), fontScaleMode(), landscapeFontScaleMode());
		ss << ";\n";
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uiGenerateIOSCommonCode(const ProjectPtr & project)
{
	std::stringstream ss;
	ss << "#ifndef __fbaf7bcc319b33e8a39e16ddff3f11c7__\n";
	ss << "#define __fbaf7bcc319b33e8a39e16ddff3f11c7__\n";
	ss << '\n';
	ss << "namespace YIP\n";
	ss << "{\n";
	ss << "\tUIFont * iosGetFont(NSString * name, float size);\n";
	ss << "\tNSString * iosChooseTranslation(NSString * def, NSDictionary * strings);\n";
	ss << '\n';
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

	std::string targetPath = ".yip-ios-view-controllers/yip_ios_layout.h";
	std::string generatedPath = project->yipDirectory()->writeFile(targetPath, ss.str());

	SourceFilePtr sourceFile = project->addSourceFile(targetPath, generatedPath);
	sourceFile->setIsGenerated(true);
	sourceFile->setPlatforms(Platform::iOS);

	ss.str(std::string());
	ss << "#import <UIKit/UIKit.h>\n";
	ss << '\n';
	ss << "namespace YIP\n";
	ss << "{\n";
	ss << "\tstatic NSMutableDictionary * g_Fonts;\n";
	ss << '\n';
	ss << "\tUIFont * iosGetFont(NSString * name, float size)\n";
	ss << "\t{\n";
	ss << "\t\tif (!g_Fonts)\n";
	ss << "\t\t\tg_Fonts = [[NSMutableDictionary dictionaryWithCapacity:8] retain];\n";
	ss << '\n';
	ss << "\t\tNSMutableDictionary * sizes = [g_Fonts objectForKey:name];\n";
	ss << "\t\tif (!sizes)\n";
	ss << "\t\t{\n";
	ss << "\t\t\tsizes = [NSMutableDictionary dictionaryWithCapacity:16];\n";
	ss << "\t\t\t[g_Fonts setObject:sizes forKey:name];\n";
	ss << "\t\t}\n";
	ss << '\n';
	ss << "\t\tNSNumber * fontSize = [NSNumber numberWithFloat:size];\n";
	ss << "\t\tUIFont * font = [sizes objectForKey:fontSize];\n";
	ss << "\t\tif (font)\n";
	ss << "\t\t\treturn font;\n";
	ss << '\n';
	ss << "\t\tfont = [UIFont fontWithName:name size:size];\n";
	ss << "\t\t[sizes setObject:font forKey:fontSize];\n";
	ss << '\n';
	ss << "\t\treturn font;\n";
	ss << "\t}\n";
	ss << '\n';
	ss << "\tNSString * iosChooseTranslation(NSString * def, NSDictionary * strings)\n";
	ss << "\t{\n";
	ss << "\t\tNSArray * languages = [NSLocale preferredLanguages];\n";
	ss << "\t\tfor (NSString * language : languages)\n";
	ss << "\t\t{\n";
	ss << "\t\t\tNSString * message = [strings objectForKey:language];\n";
	ss << "\t\t\tif (message)\n";
	ss << "\t\t\t\treturn message;\n";
	ss << "\t\t}\n";
	ss << "\t\tfor (NSString * language : languages)\n";
	ss << "\t\t{\n";
	ss << "\t\t\tNSString * message = [strings objectForKey:[language substringToIndex:2]];\n";
	ss << "\t\t\tif (message)\n";
	ss << "\t\t\t\treturn message;\n";
	ss << "\t\t}\n";
	ss << "\t\treturn def;\n";
	ss << "\t}\n";
	ss << "}\n";

	targetPath = ".yip-ios-view-controllers/ios_layout.mm";
	generatedPath = project->yipDirectory()->writeFile(targetPath, ss.str());

	sourceFile = project->addSourceFile(targetPath, generatedPath);
	sourceFile->setIsGenerated(true);
	sourceFile->setPlatforms(Platform::iOS);
}

void uiGenerateIOSViewController(UILayoutMap & layouts, const ProjectPtr & project,
	const Project::IOSViewController & cntrl)
{
	std::string yipDir = project->yipDirectory()->path();

	SourceFilePtr sourceFileH;
	SourceFilePtr sourceFileM;

	std::string targetName = cntrl.name;
	std::string targetPathH = pathConcat(".yip-ios-view-controllers/yip-ios", targetName) + ".h";
	std::string targetPathM = pathConcat(".yip-ios-view-controllers/yip-ios", targetName) + ".mm";

	bool shouldProcessFile =
		(cntrl.iphone.get() && (
			project->yipDirectory()->shouldProcessFile(targetPathH, cntrl.iphone->path(), true) ||
			project->yipDirectory()->shouldProcessFile(targetPathM, cntrl.iphone->path(), true))) ||
		(cntrl.ipad.get() && (
			project->yipDirectory()->shouldProcessFile(targetPathH, cntrl.ipad->path(), true) ||
			project->yipDirectory()->shouldProcessFile(targetPathM, cntrl.ipad->path(), true)));

	if (!shouldProcessFile)
	{
		for (auto it : project->translationFiles())
		{
			if (project->yipDirectory()->shouldProcessFile(targetPathH, it.second->path(), false) ||
				project->yipDirectory()->shouldProcessFile(targetPathM, it.second->path(), false))
			{
				shouldProcessFile = true;
				break;
			}
		}
	}

	if (!shouldProcessFile)
	{
		sourceFileH = project->addSourceFile(targetPathH, pathConcat(yipDir, targetPathH));
		sourceFileM = project->addSourceFile(targetPathM, pathConcat(yipDir, targetPathM));
	}
	else
	{
		UILayoutPtr iphoneLayout = uiLoadLayout(layouts, cntrl.iphone);
		UILayoutPtr ipadLayout = uiLoadLayout(layouts, cntrl.ipad);

		UIWidgetInfos widgetInfos = uiGetWidgetInfos({
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
		sm << "#import \"../yip_ios_layout.h\"\n";
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
				widget->iosGenerateInitCode(project, "\t\t\t", sm);
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
				widget->iosGenerateInitCode(project, "\t\t\t", sm);
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
		sm << "\tCGRect frame = self.view.bounds;\n";
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
			if (ipadLayout->allowPortrait())
				sm << " | UIInterfaceOrientationMaskPortrait | UIInterfaceOrientationMaskPortraitUpsideDown";
			if (ipadLayout->allowLandscape())
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
