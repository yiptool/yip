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
#include "widgets/ui_image_view.h"
#include "widgets/ui_webview.h"
#include "widgets/ui_spinner.h"
#include "widgets/ui_switch.h"
#include "widgets/ui_scroll_view.h"
#include "widgets/ui_text_field.h"
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
	case UIScaleNone: return "1.0f";
	}

	assert(false);
	throw std::runtime_error("invalid scale mode.");
}

std::string iosTextAlignment(UITextAlignment align)
{
	switch (align)
	{
	case UITextAlignUnspecified: return "NSTextAlignmentLeft";
	case UITextAlignLeft: return "NSTextAlignmentLeft";
	case UITextAlignRight: return "NSTextAlignmentRight";
	case UITextAlignCenter: return "NSTextAlignmentCenter";
	}

	assert(false);
	throw std::runtime_error("invalid text alignment.");
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

	ss << "iosChooseTranslation(@\"";
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
	ss << "iosGetFont(@\"";
	cxxEscape(ss, font->family);
	ss << "\", (landscape ? " << font->landscapeSize << " * " << iosScaleFunc(landscapeScaleMode, false);
	ss << " : " << font->size << " * " << iosScaleFunc(scaleMode, false) << "))";
}

void iosGetImage(std::stringstream & ss, const UIImagePtr & image)
{
	ss << "iosImageFromResource(@\"";
	cxxEscape(ss, image->name);
	ss << "\")";
}

void iosGetScaledImage(const UIWidget * wd, std::stringstream & ss, const UIImagePtr & image,
	const std::string & imageObject, bool keepAspectRatio)
{
	std::string wMode1 = iosScaleFunc(wd->widthScaleMode(), true);
	std::string hMode1 = iosScaleFunc(wd->heightScaleMode(), false);

	std::string wMode2 = iosScaleFunc(wd->landscapeWidthScaleMode(), true);
	std::string hMode2 = iosScaleFunc(wd->landscapeHeightScaleMode(), false);

	std::string wMode = fmt() << "(landscape ? " << wMode2 << " : " << wMode1 << ')';
	std::string hMode = fmt() << "(landscape ? " << hMode2 << " : " << hMode1 << ')';

	if (!image->isNinePatch)
	{
		if (!keepAspectRatio)
			ss << "iosScaledImage(" << imageObject << ", " << wMode << ", " << hMode << ')';
		else
		{
			std::string scaleMode = fmt() << "std::min(" << wMode << ", " << hMode << ')';
			ss << "iosScaledImage(" << imageObject << ", " << scaleMode << ", " << scaleMode << ')';
		}
	}
	else
	{
		std::string scaleMode = fmt() << "std::min(" << wMode << ", " << hMode << ')';
		ss << "iosScaledImageWithCapInsets(" << imageObject << ", " << scaleMode << ", " << scaleMode << ", "
			<< image->left << ", " << image->top << ", " << image->right << ", " << image->bottom << ')';
	}
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
		{
			if (it.first.length() == 8 && it.first == "darkgray")
				return fmt() << "[UIColor darkGrayColor]";
			else if (it.first.length() == 9 && it.first == "lightgray")
				return fmt() << "[UIColor lightGrayColor]";
			else
				return fmt() << "[UIColor " << it.first << "Color]";
		}
	}

	std::stringstream ss;
	ss << "[UIColor colorWithRed:" << r / 255.0f << " green:" << g / 255.0f << " blue:" << b / 255.0f
		<< " alpha:" << a / 255.0f << "]";

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
	ss << prefix << id() << " = [[" << iosClassName() << " alloc] initWithFrame:CGRectZero];\n";
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
// UIScrollView

void UIScrollView::iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss)
{
	UIGroup::iosGenerateInitCode(project, prefix, ss);
}

void UIScrollView::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIGroup::iosGenerateLayoutCode(prefix, ss);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UILabel

void UILabel::iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << id() << " = [[UILabel alloc] initWithFrame:CGRectZero];\n";
	UIWidget::iosGenerateInitCode(project, prefix, ss);

	if (!text().empty())
	{
		ss << prefix << id() << ".text = ";
		iosChooseTranslation(project, prefix, ss, text());
		ss << ";\n";
	}

	ss << prefix << id() << ".textColor = " << textColor().iosValue() << ";\n";
}

void UILabel::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::iosGenerateLayoutCode(prefix, ss);

	if (font().get())
	{
		ss << prefix << id() << ".font = ";
		iosGetFont(ss, font(), fontScaleMode(), landscapeFontScaleMode());
		ss << ";\n";
	}

	ss << prefix << id() << ".textAlignment = " << iosTextAlignment(m_TextAlignment) << ";\n";
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIImageView

void UIImageView::iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
	std::stringstream & ss)
{
	ss << prefix << id() << " = [[UIImageView alloc] initWithImage:nil];\n";
	UIWidget::iosGenerateInitCode(project, prefix, ss);

	if (m_Image.get())
	{
		ss << prefix << "objc_setAssociatedObject(" << id() << ", &YIP::KEY_IMAGE, ";
		iosGetImage(ss, m_Image);
		ss << ", OBJC_ASSOCIATION_RETAIN_NONATOMIC);\n";
	}
}

void UIImageView::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::iosGenerateLayoutCode(prefix, ss);

	if (m_Image.get())
	{
		ss << prefix << id() << ".image = ";
		iosGetScaledImage(this, ss, m_Image,
			fmt() << "objc_getAssociatedObject(" << id() << ", &YIP::KEY_IMAGE)", false);
		ss << ";\n";
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UITextField

void UITextField::iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << id() << " = [[UITextField alloc] initWithFrame:CGRectZero];\n";
	UIWidget::iosGenerateInitCode(project, prefix, ss);

	if (!text().empty())
	{
		ss << prefix << id() << ".placeholder = ";
		iosChooseTranslation(project, prefix, ss, text());
		ss << ";\n";
	}

	ss << prefix << id() << ".textColor = " << textColor().iosValue() << ";\n";
}

void UITextField::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::iosGenerateLayoutCode(prefix, ss);

	if (font().get())
	{
		ss << prefix << id() << ".font = ";
		iosGetFont(ss, font(), fontScaleMode(), landscapeFontScaleMode());
		ss << ";\n";
	}

	ss << prefix << id() << ".textAlignment = " << iosTextAlignment(m_TextAlignment) << ";\n";
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UISwitch

void UISwitch::iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss)
{
	if (!isCustom())
		ss << prefix << id() << " = [[UISwitch alloc] initWithFrame:CGRectZero];\n";
	else
	{
		ss << prefix << id() << " = [[NZSwitchControl alloc] init];\n";

		ss << prefix << id() << ".knob.image = iosImageFromResource(@\"";
		cxxEscape(ss, m_KnobImage);
		ss << "\");\n";

		ss << prefix << id() << ".turnedOn.image = iosImageFromResource(@\"";
		cxxEscape(ss, m_OnImage);
		ss << "\");\n";

		ss << prefix << id() << ".turnedOff.image = iosImageFromResource(@\"";
		cxxEscape(ss, m_OffImage);
		ss << "\");\n";
	}

	UIWidget::iosGenerateInitCode(project, prefix, ss);
}

void UISwitch::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::iosGenerateLayoutCode(prefix, ss);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UISpinner

void UISpinner::iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << id() << " = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:";
	switch (m_Style)
	{
	case SmallDark: ss << "UIActivityIndicatorViewStyleGray"; break;
	case SmallLight: ss << "UIActivityIndicatorViewStyleWhite"; break;
	case LargeLight: ss << "UIActivityIndicatorViewStyleWhiteLarge"; break;
	}
	ss << "];\n";
	UIWidget::iosGenerateInitCode(project, prefix, ss);
}

void UISpinner::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::iosGenerateLayoutCode(prefix, ss);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIButton

void UIButton::iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << id() << " = [[UIButton buttonWithType:UIButtonTypeCustom] retain];\n";
	UIWidget::iosGenerateInitCode(project, prefix, ss);

	ss << prefix << id() << ".imageView.contentMode = UIViewContentModeScaleAspectFit;\n";

	if (!text().empty())
	{
		ss << prefix << '[' << id() << " setTitle:";
		iosChooseTranslation(project, prefix, ss, text());
		ss << " forState:UIControlStateNormal];\n";
	}

	ss << prefix << '[' << id() << " setTitleColor:" << textColor().iosValue()
		<< " forState:UIControlStateNormal];\n";

	if (m_Image.get())
	{
		ss << prefix << "objc_setAssociatedObject(" << id() << ", &YIP::KEY_IMAGE, ";
		iosGetImage(ss, m_Image);
		ss << ", OBJC_ASSOCIATION_RETAIN_NONATOMIC);\n";
	}
}

void UIButton::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::iosGenerateLayoutCode(prefix, ss);

	std::string wMode1 = iosScaleFunc(widthScaleMode(), true);
	std::string hMode1 = iosScaleFunc(heightScaleMode(), false);
	std::string wMode2 = iosScaleFunc(landscapeWidthScaleMode(), true);
	std::string hMode2 = iosScaleFunc(landscapeHeightScaleMode(), false);
	std::string wMode = fmt() << "(landscape ? " << wMode2 << " : " << wMode1 << ')';
	std::string hMode = fmt() << "(landscape ? " << hMode2 << " : " << hMode1 << ')';
	std::string scaleMode = fmt() << "std::min(" << wMode << ", " << hMode << ')';
	ss << prefix << id() << ".imageEdgeInsets = UIEdgeInsetsMake(0, 0, 0, 7.0f * " << scaleMode << ");\n";
	ss << prefix << id() << ".titleEdgeInsets = UIEdgeInsetsMake(0, 7.0f * " << scaleMode << ", 0, 0);\n";

	if (font().get())
	{
		ss << prefix << id() << ".titleLabel.font = ";
		iosGetFont(ss, font(), fontScaleMode(), landscapeFontScaleMode());
		ss << ";\n";
	}

	if (m_Image.get())
	{
		ss << prefix << '[' << id() << " setImage:";
		iosGetScaledImage(this, ss, m_Image,
			fmt() << "objc_getAssociatedObject(" << id() << ", &YIP::KEY_IMAGE)", true);
		ss << " forState:UIControlStateNormal];\n";
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIWebView

void UIWebView::iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << id() << " = [[UIWebView alloc] init];\n";
	UIWidget::iosGenerateInitCode(project, prefix, ss);
}

void UIWebView::iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::iosGenerateLayoutCode(prefix, ss);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

		std::set<std::string> stringIDs;
		for (const auto & it : iphoneLayout->strings())
			stringIDs.insert(it.first);
		for (const auto & it : ipadLayout->strings())
			stringIDs.insert(it.first);

		UIWidgetInfos widgetInfos = uiGetWidgetInfos({
			// Don't change order of this items: it's important
			iphoneLayout,
			ipadLayout,
		}, false);

		bool hasIPhone = iphoneLayout.get() != nullptr;
		bool hasIPad = ipadLayout.get() != nullptr;

		std::stringstream sh;
		sh << "#import <UIKit/UIKit.h>\n";
		sh << "#import <yip-imports/ios/NZSwitchControl.h>\n";
		sh << "#import <yip-imports/ios/image.h>\n";
		sh << "#import <yip-imports/TPKeyboardAvoiding/TPKeyboardAvoidingScrollView.h>\n";
		sh << "#import <objc/runtime.h>\n";
		sh << '\n';
		sh << "@interface " << cntrl.name << " : " << cntrl.parentClass << "\n";
		for (const auto & it : widgetInfos)
		{
			const UIWidgetPtr & widget = (it.second.iphone.get() ? it.second.iphone : it.second.ipad);
			sh << "@property (nonatomic, readonly, retain) " << widget->iosClassName()
				<< " * " << it.first << ";\n";
		}
		for (const auto & it : stringIDs)
			sh << "@property (nonatomic, readonly, copy) NSString * " << it << ";\n";
		sh << "-(id)init;\n";
		sh << "-(void)dealloc;\n";
		sh << "@end\n";

		std::stringstream sm;
		sm << "#import \"" << targetName << ".h\"\n";
		sm << "#import <yip-imports/ios/i18n.h>\n";
		sm << "#import <yip-imports/ios/resource.h>\n";
		sm << "#import <yip-imports/ios/font.h>\n";
		sm << '\n';
		sm << "namespace YIP\n";
		sm << "{\n";
		sm << "\tstatic char KEY_IMAGE;\n";
		sm << '\n';
		sm << "\ttemplate <unsigned char ALIGN> CGRect iosLayoutRect(float x, float y, float w, float h,\n";
		sm << "\t\tfloat xScale, float yScale, float wScale, float hScale, float horzScale, float vertScale)\n";
		sm << "\t{\n";
		sm << "\t\t(void)KEY_IMAGE; /* Prevent compiler warning. */\n";
		sm << '\n';
		sm << "\t\tfloat widgetX = x * xScale;\n";
		sm << "\t\tfloat widgetY = y * yScale;\n";
		sm << "\t\tfloat widgetW = w * wScale;\n";
		sm << "\t\tfloat widgetH = h * hScale;\n";
		sm << '\n';
		sm << "\t\tif ((ALIGN & " << UIAlignHorizontalMask << ") == " << UIAlignHCenter << ")\n";
		sm << "\t\t\twidgetX += (w * horzScale - widgetW) * 0.5f;\n";
		sm << "\t\telse if ((ALIGN & " << UIAlignHorizontalMask << ") == " << UIAlignRight << ")\n";
		sm << "\t\t\twidgetX += w * horzScale - widgetW;\n";
		sm << '\n';
		sm << "\t\tif ((ALIGN & " << UIAlignVerticalMask << ") == " << UIAlignVCenter << ")\n";
		sm << "\t\t\twidgetY += (h * vertScale - widgetH) * 0.5f;\n";
		sm << "\t\telse if ((ALIGN & " << UIAlignVerticalMask << ") == " << UIAlignBottom << ")\n";
		sm << "\t\t\twidgetY += h * vertScale - widgetH;\n";
		sm << '\n';
		sm << "\t\treturn CGRectMake(widgetX, widgetY, widgetW, widgetH);\n";
		sm << "\t}\n";
		sm << "}\n";
		sm << '\n';
		sm << "@implementation " << cntrl.name << '\n';
		sm << '\n';
		for (const auto & it : widgetInfos)
			sm << "@synthesize " << it.first << ";\n";
		for (const auto & it : stringIDs)
			sm << "@synthesize " << it << ";\n";
		sm << '\n';
		sm << "-(id)init\n";
		sm << "{\n";
		sm << "\tself = [super init];\n";
		sm << "\tif (self)\n";
		sm << "\t{\n";
		if (hasIPhone)
		{
			sm << '\n';
			sm << "\t\tif (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)\n";
			sm << "\t\t{\n";
			for (const auto & it : iphoneLayout->strings())
			{
				sm << "\t\t\t" << it.first << " = ";
				iosChooseTranslation(project, "\t\t\t", sm, it.second);
				sm << ";\n";
			}
			for (const UIWidgetPtr & widget : iphoneLayout->widgets())
			{
				widget->iosGenerateInitCode(project, "\t\t\t", sm);
				sm << "\t\t\t[self.view addSubview:" << widget->id() << "];\n";
			}
			sm << "\t\t}\n";
		}
		if (hasIPad)
		{
			sm << '\n';
			sm << "\t\tif (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)\n";
			sm << "\t\t{\n";
			for (const auto & it : ipadLayout->strings())
			{
				sm << "\t\t\t" << it.first << " = ";
				iosChooseTranslation(project, "\t\t\t", sm, it.second);
				sm << ";\n";
			}
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
		for (const auto & it : stringIDs)
		{
			sm << "\t[" << it << " release];\n";
			sm << "\t" << it << " = nil;\n";
		}
		for (const auto & it : widgetInfos)
		{
			sm << "\t[" << it.first << " release];\n";
			sm << "\t" << it.first << " = nil;\n";
		}
		sm << "\t[super dealloc];\n";
		sm << "}\n";
		sm << '\n';
		sm << "-(void)viewWillAppear:(BOOL)animated\n";
		sm << "{\n";
		sm << "\t[super viewWillAppear:animated];\n";
		sm << "\tif ([self respondsToSelector:@selector(prefersStatusBarHidden)])\n";
		sm << "\t{\n";
		sm << "\t\t[[UIApplication sharedApplication] setStatusBarHidden:[self prefersStatusBarHidden]\n";
		sm << "\t\t\twithAnimation:[self preferredStatusBarUpdateAnimation]];\n";
		sm << "\t\tif ([self respondsToSelector:@selector(setNeedsStatusBarAppearanceUpdate)])\n";
		sm << "\t\t\t[self setNeedsStatusBarAppearanceUpdate];\n";
		sm << "\t}\n";
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
			if (ipadLayout->allowPortraitTablet())
				sm << " | UIInterfaceOrientationMaskPortrait | UIInterfaceOrientationMaskPortraitUpsideDown";
			if (ipadLayout->allowLandscapeTablet())
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
