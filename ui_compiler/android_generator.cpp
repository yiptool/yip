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
#include "android_generator.h"
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
#include "widgets/ui_table_view.h"
#include "widgets/ui_text_field.h"
#include "../util/cxx-util/cxx-util/fmt.h"
#include "../util/cxx-util/cxx-util/replace.h"
#include "../util/path-util/path-util.h"
#include "../util/sha1.h"
#include "../util/java_escape.h"
#include "../util/xml.h"
#include <cassert>
#include <stdexcept>
#include <iomanip>

std::string androidScaleFunc(UIScaleMode mode, bool horz)
{
	switch (mode)
	{
	case UIScaleDefault: return (horz ? "horzScale" : "vertScale");
	case UIScaleMin: return "Math.min(horzScale, vertScale)";
	case UIScaleMax: return "Math.max(horzScale, vertScale)";
	case UIScaleHorz: return "horzScale";
	case UIScaleVert: return "vertScale";
	case UIScaleAvg: return "((horzScale + vertScale) * 0.5f)";
	case UIScaleNone: return "1.0f";
	}

	assert(false);
	throw std::runtime_error("invalid scale mode.");
}

std::string androidTextAlignment(UITextAlignment align)
{
	switch (align)
	{
	case UITextAlignUnspecified: return "TEXT_ALIGNMENT_VIEW_START";
	case UITextAlignLeft: return "TEXT_ALIGNMENT_VIEW_START";
	case UITextAlignRight: return "TEXT_ALIGNMENT_VIEW_END";
	case UITextAlignCenter: return "TEXT_ALIGNMENT_CENTER";
	}

	assert(false);
	throw std::runtime_error("invalid text alignment.");
}

void androidChooseTranslation(const ProjectPtr & project, std::stringstream & ss,
	const std::string & text, std::map<std::string, std::string> & translations)
{
	if (project->translationFiles().size() == 0)
	{
		ss << "\"";
		javaEscape(ss, text);
		ss << '"';
		return;
	}

	std::string id = "YIP_" + sha1(text);
	translations.insert(std::make_pair(id, text));

	ss << "getContext().getString(R.string." << id << ')';
}

void androidGenerateLayoutCode(const UIWidget * wd, const std::string & prefix, std::stringstream & ss, bool landscape)
{
	UIAlignment alignment = (!landscape ? wd->alignment() : wd->landscapeAlignment());

	float x = (!landscape ? wd->x() : wd->landscapeX());
	float y = (!landscape ? wd->y() : wd->landscapeY());
	float w = (!landscape ? wd->width() : wd->landscapeWidth());
	float h = (!landscape ? wd->height() : wd->landscapeHeight());

	std::string xMode = androidScaleFunc(!landscape ? wd->xScaleMode() : wd->landscapeXScaleMode(), true);
	std::string yMode = androidScaleFunc(!landscape ? wd->yScaleMode() : wd->landscapeYScaleMode(), false);
	std::string wMode = androidScaleFunc(!landscape ? wd->widthScaleMode() : wd->landscapeWidthScaleMode(), true);
	std::string hMode = androidScaleFunc(!landscape ? wd->heightScaleMode() : wd->landscapeHeightScaleMode(), false);

	ss << prefix << "ru.zapolnov.yip.Util.layoutChild(" << wd->id() << ", " << alignment << ", " << x << ", "
		<< y << ", " << w << ", " << h << ", " << xMode << ", " << yMode << ", " << wMode << ", " << hMode
		<< ", horzScale, vertScale);\n";
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIColor

std::string UIColor::androidValue() const
{
	std::stringstream ss;
	ss << "0x";
	ss << std::hex << std::setw(2) << std::setfill('0') << int(a);
	ss << std::hex << std::setw(2) << std::setfill('0') << int(r);
	ss << std::hex << std::setw(2) << std::setfill('0') << int(g);
	ss << std::hex << std::setw(2) << std::setfill('0') << int(b);
	return ss.str();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIWidget

void UIWidget::androidGenerateInitCode(const ProjectPtr &, const std::string & prefix, std::stringstream & ss,
	std::map<std::string, std::string> &)
{
	ss << prefix << m_ID << ".setBackgroundColor(" << m_BackgroundColor.androidValue() << ");\n";
}

void UIWidget::androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	ss << prefix << "if (landscape)\n";
	::androidGenerateLayoutCode(this, prefix + "\t", ss, true);
	ss << prefix << "else\n";
	::androidGenerateLayoutCode(this, prefix + "\t", ss, false);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIGroup

void UIGroup::androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
	std::stringstream & ss, std::map<std::string, std::string> & translations)
{
	ss << prefix << id() << " = new DummyViewGroup(getContext());\n";
	UIWidget::androidGenerateInitCode(project, prefix, ss, translations);

	for (const UIWidgetPtr & widget : m_Widgets)
	{
		widget->androidGenerateInitCode(project, prefix, ss, translations);
		ss << prefix << id() << ".addView(" << widget->id() << ");\n";
	}
}

void UIGroup::androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::androidGenerateLayoutCode(prefix, ss);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIScrollView

void UIScrollView::androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
	std::stringstream & ss, std::map<std::string, std::string> & translations)
{
	UIGroup::androidGenerateInitCode(project, prefix, ss, translations);
}

void UIScrollView::androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIGroup::androidGenerateLayoutCode(prefix, ss);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UITableView

void UITableView::androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
	std::stringstream & ss, std::map<std::string, std::string> & translations)
{
	UIGroup::androidGenerateInitCode(project, prefix, ss, translations);
}

void UITableView::androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIGroup::androidGenerateLayoutCode(prefix, ss);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UILabel

void UILabel::androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
	std::stringstream & ss, std::map<std::string, std::string> & translations)
{
	ss << prefix << id() << " = new " << androidClassName() << "(getContext());\n";
	UIWidget::androidGenerateInitCode(project, prefix, ss, translations);

	if (!text().empty())
	{
		ss << prefix << id() << ".setText(";
		androidChooseTranslation(project, ss, text(), translations);
		ss << ");\n";
	}

	if (font().get())
	{
		ss << prefix << id() << ".setTypeface(ru.zapolnov.yip.Util.getTypeface(getResources().getAssets(), \"";
		javaEscape(ss, font()->family);
		ss << ".ttf\"));\n";
	}

	ss << prefix << id() << ".setTextColor(" << textColor().androidValue() << ");\n";
}

void UILabel::androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::androidGenerateLayoutCode(prefix, ss);

	if (font().get())
	{
		ss << prefix << id() << ".setTextSize(TypedValue.COMPLEX_UNIT_PT, landscape ? " << font()->landscapeSize << " * "
			<< androidScaleFunc(landscapeFontScaleMode(), false) << " : " << font()->size << " * "
			<< androidScaleFunc(fontScaleMode(), false) << ");\n";
	}

	ss << prefix << id() << ".setTextAlignment(" << androidTextAlignment(m_TextAlignment) << ");\n";
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIImageView

void UIImageView::androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
	std::stringstream & ss, std::map<std::string, std::string> & translations)
{
	ss << prefix << id() << " = new " << androidClassName() << "(getContext());\n";
	UIWidget::androidGenerateInitCode(project, prefix, ss, translations);

/*
	if (m_Image.get())
	{
		ss << prefix << "objc_setAssociatedObject(" << id() << ", &YIP::KEY_IMAGE, ";
		androidGetImage(ss, m_Image);
		ss << ", OBJC_ASSOCIATION_RETAIN_NONATOMIC);\n";
	}
*/
}

void UIImageView::androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::androidGenerateLayoutCode(prefix, ss);

/*
	if (m_Image.get())
	{
		ss << prefix << id() << ".image = ";
		androidGetScaledImage(this, ss, m_Image, fmt() << "objc_getAssociatedObject(" << id() << ", &YIP::KEY_IMAGE)");
		ss << ";\n";
	}
*/
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UITextField

void UITextField::androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
	std::stringstream & ss, std::map<std::string, std::string> & translations)
{
	ss << prefix << id() << " = new " << androidClassName() << "(getContext());\n";
	UIWidget::androidGenerateInitCode(project, prefix, ss, translations);

	if (!text().empty())
	{
		ss << prefix << id() << ".setHint(";
		androidChooseTranslation(project, ss, text(), translations);
		ss << ");\n";
	}

	if (font().get())
	{
		ss << prefix << id() << ".setTypeface(ru.zapolnov.yip.Util.getTypeface(getResources().getAssets(), \"";
		javaEscape(ss, font()->family);
		ss << ".ttf\"));\n";
	}

	ss << prefix << id() << ".setTextColor(" << textColor().androidValue() << ");\n";
}

void UITextField::androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::androidGenerateLayoutCode(prefix, ss);

	if (font().get())
	{
		ss << prefix << id() << ".setTextSize(TypedValue.COMPLEX_UNIT_PT, landscape ? "
			<< font()->landscapeSize << " * " << androidScaleFunc(landscapeFontScaleMode(), false) << " : "
			<< font()->size << " * " << androidScaleFunc(fontScaleMode(), false) << ");\n";
	}

	ss << prefix << id() << ".setTextAlignment(" << androidTextAlignment(m_TextAlignment) << ");\n";
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UISwitch

void UISwitch::androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
	std::stringstream & ss, std::map<std::string, std::string> & translations)
{
	ss << prefix << id() << " = new " << androidClassName() << "(getContext());\n";
/*
	if (!isCustom())
		ss << prefix << id() << " = [[UISwitch alloc] initWithFrame:CGRectZero];\n";
	else
	{
		ss << prefix << id() << " = [[NZSwitchControl alloc] init];\n";

		ss << prefix << id() << ".knob.image = androidImageFromResource(@\"";
		cxxEscape(ss, m_KnobImage);
		ss << "\");\n";

		ss << prefix << id() << ".turnedOn.image = androidImageFromResource(@\"";
		cxxEscape(ss, m_OnImage);
		ss << "\");\n";

		ss << prefix << id() << ".turnedOff.image = androidImageFromResource(@\"";
		cxxEscape(ss, m_OffImage);
		ss << "\");\n";
	}
*/

	UIWidget::androidGenerateInitCode(project, prefix, ss, translations);
}

void UISwitch::androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::androidGenerateLayoutCode(prefix, ss);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UISpinner

void UISpinner::androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
	std::stringstream & ss, std::map<std::string, std::string> & translations)
{
	// FIXME: style
	ss << prefix << id() << " = new android.widget.Spinner(getContext());\n";
	UIWidget::androidGenerateInitCode(project, prefix, ss, translations);
}

void UISpinner::androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::androidGenerateLayoutCode(prefix, ss);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIButton

void UIButton::androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
	std::stringstream & ss, std::map<std::string, std::string> & translations)
{
	ss << prefix << id() << " = new " << androidClassName() << "(getContext());\n";
	UIWidget::androidGenerateInitCode(project, prefix, ss, translations);

	if (!text().empty())
	{
		ss << prefix << id() << ".setText(";
		androidChooseTranslation(project, ss, text(), translations);
		ss << ");\n";
	}

	ss << prefix << id() << ".setTextColor(" << textColor().androidValue() << ");\n";

	if (font().get())
	{
		ss << prefix << id() << ".setTypeface(ru.zapolnov.yip.Util.getTypeface(getResources().getAssets(), \"";
		javaEscape(ss, font()->family);
		ss << ".ttf\"));\n";
	}

/*
	if (m_Image.get())
	{
		ss << prefix << "objc_setAssociatedObject(" << id() << ", &YIP::KEY_IMAGE, ";
		androidGetImage(ss, m_Image);
		ss << ", OBJC_ASSOCIATION_RETAIN_NONATOMIC);\n";
	}

	if (m_BackgroundImage.get())
	{
		ss << prefix << "objc_setAssociatedObject(" << id() << ", &YIP::KEY_IMAGE_2, ";
		androidGetImage(ss, m_BackgroundImage);
		ss << ", OBJC_ASSOCIATION_RETAIN_NONATOMIC);\n";
	}
*/
}

void UIButton::androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::androidGenerateLayoutCode(prefix, ss);

	if (font().get())
	{
		ss << prefix << id() << ".setTextSize(TypedValue.COMPLEX_UNIT_PT, landscape ? " << font()->landscapeSize << " * "
			<< androidScaleFunc(landscapeFontScaleMode(), false) << " : " << font()->size << " * "
			<< androidScaleFunc(fontScaleMode(), false) << ");\n";
	}

/*
	if (m_Image.get())
	{
		ss << prefix << '[' << id() << " setImage:";
		androidGetScaledImage(this, ss, m_Image, fmt() << "objc_getAssociatedObject(" << id() << ", &YIP::KEY_IMAGE)");
		ss << " forState:UIControlStateNormal];\n";
	}

	if (m_BackgroundImage.get())
	{
		ss << prefix << '[' << id() << " setBackgroundImage:";
		androidGetScaledImage(this, ss, m_BackgroundImage,
			fmt() << "objc_getAssociatedObject(" << id() << ", &YIP::KEY_IMAGE_2)", false);
		ss << " forState:UIControlStateNormal];\n";
	}
*/
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UIWebView

void UIWebView::androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
	std::stringstream & ss, std::map<std::string, std::string> & translations)
{
	ss << prefix << id() << " = new " << androidClassName() << "(getContext());\n";
	UIWidget::androidGenerateInitCode(project, prefix, ss, translations);
}

void UIWebView::androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss)
{
	UIWidget::androidGenerateLayoutCode(prefix, ss);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uiGenerateAndroidView(UILayoutMap & layouts, const ProjectPtr & project,
	const Project::AndroidView & view, std::map<std::string, std::string> & translations)
{
	std::string yipDir = project->yipDirectory()->path();

	std::string targetName = replace(view.name, '.', "/");
	std::string targetPath = pathConcat(".yip-android", targetName) + ".java";

	UILayoutPtr phoneLayout = uiLoadLayout(layouts, view.phone);
	UILayoutPtr tablet7Layout = uiLoadLayout(layouts, view.tablet7);
	UILayoutPtr tablet10Layout = uiLoadLayout(layouts, view.tablet10);

	std::set<std::string> stringIDs;
	for (const auto & it : phoneLayout->strings())
		stringIDs.insert(it.first);
	for (const auto & it : tablet7Layout->strings())
		stringIDs.insert(it.first);
	for (const auto & it : tablet10Layout->strings())
		stringIDs.insert(it.first);

	UIWidgetInfos widgetInfos = uiGetWidgetInfos({
		// Don't change order of this items: it's important
		phoneLayout,
		tablet7Layout,
		tablet10Layout
	}, true);

	bool hasPhone = phoneLayout.get() != nullptr;
	bool hasTablet7 = tablet7Layout.get() != nullptr;
	bool hasTablet10 = tablet10Layout.get() != nullptr;

	std::string className, packageName;
	size_t pos = view.name.rfind('.');
	if (pos == std::string::npos)
		className = view.name;
	else if (pos == 0)
	{
		className = view.name.substr(1);
		packageName = project->androidPackage();
	}
	else
	{
		className = view.name.substr(pos + 1);
		packageName = view.name.substr(0, pos);
	}

	std::stringstream ss;
	ss << '\n';
	if (!packageName.empty())
	{
		ss << "package " << packageName << ";\n";
		ss << '\n';
	}
	ss << "import android.content.Context;\n";
	ss << "import android.content.res.Configuration;\n";
	ss << "import android.view.View;\n";
	ss << "import android.view.View.MeasureSpec;\n";
	ss << "import android.view.ViewGroup;\n";
	ss << "import android.util.AttributeSet;\n";
	ss << "import android.util.TypedValue;\n";
	ss << "import ru.zapolnov.yip.DummyViewGroup;\n";
	ss << "import " << project->androidPackage() << ".R;\n";
	ss << '\n';
	ss << "public class " << className << " extends ViewGroup\n";
	ss << "{\n";
	for (const auto & it : stringIDs)
		ss << "\tpublic final String " << it << ";\n";
	for (auto it : widgetInfos)
	{
		const UIWidgetPtr & widget = (it.second.phone.get() ? it.second.phone :
			(it.second.tablet7.get() ? it.second.tablet7 : it.second.tablet10));
		ss << "\tpublic final " << widget->androidClassName() << ' ' << it.first << ";\n";
	}
	ss << '\n';
	ss << "\tpublic " << className << "(Context context)\n";
	ss << "\t{\n";
	ss << "\t\tthis(context, null, 0);\n";
	ss << "\t}\n";
	ss << '\n';
	ss << "\tpublic " << className << "(Context context, AttributeSet attrs)\n";
	ss << "\t{\n";
	ss << "\t\tthis(context, attrs, 0);\n";
	ss << "\t}\n";
	ss << '\n';
	ss << "\tpublic " << className << "(Context context, AttributeSet attrs, int defStyle)\n";
	ss << "\t{\n";
	ss << "\t\tsuper(context, attrs, defStyle);\n";
	bool first = true;
	if (hasPhone)
	{
		ss << (first ? "\n\t\t" : "\t\telse ");
		ss << "if (\"P\".equals(getContext().getString(R.string.YIP_screen_type)))\n";
		ss << "\t\t{\n";
		for (const auto & it : stringIDs)
		{
			auto jt = phoneLayout->strings().find(it);
			ss << "\t\t\t" << it << " = ";
			if (jt == phoneLayout->strings().end())
				ss << "null;";
			else
				androidChooseTranslation(project, ss, jt->second, translations);
			ss << ";\n";
		}
		for (const UIWidgetPtr & widget : phoneLayout->widgets())
		{
			widget->androidGenerateInitCode(project, "\t\t\t", ss, translations);
			ss << "\t\t\taddView(" << widget->id() << ");\n";
		}
		ss << "\t\t}\n";
		first = false;
	}
	if (hasTablet7)
	{
		ss << (first ? "\n\t\t" : "\t\telse ");
		ss << "if (\"7\".equals(getContext().getString(R.string.YIP_screen_type)))\n";
		ss << "\t\t{\n";
		for (const auto & it : stringIDs)
		{
			auto jt = tablet7Layout->strings().find(it);
			ss << "\t\t\t" << it << " = ";
			if (jt == tablet7Layout->strings().end())
				ss << "null";
			else
				androidChooseTranslation(project, ss, jt->second, translations);
			ss << ";\n";
		}
		for (const UIWidgetPtr & widget : tablet7Layout->widgets())
		{
			widget->androidGenerateInitCode(project, "\t\t\t", ss, translations);
			ss << "\t\t\taddView(" << widget->id() << ");\n";
		}
		ss << "\t\t}\n";
		first = false;
	}
	if (hasTablet10)
	{
		ss << (first ? "\n\t\t" : "\t\telse ");
		ss << "if (\"T\".equals(getContext().getString(R.string.YIP_screen_type)))\n";
		ss << "\t\t{\n";
		for (const auto & it : stringIDs)
		{
			auto jt = tablet10Layout->strings().find(it);
			ss << "\t\t\t" << it << " = ";
			if (jt == tablet10Layout->strings().end())
				ss << "null";
			else
				androidChooseTranslation(project, ss, jt->second, translations);
			ss << ";\n";
		}
		for (const UIWidgetPtr & widget : tablet10Layout->widgets())
		{
			widget->androidGenerateInitCode(project, "\t\t\t", ss, translations);
			ss << "\t\t\taddView(" << widget->id() << ");\n";
		}
		ss << "\t\t}\n";
		first = false;
	}
	ss << (first ? "" : "\t\telse\n");
	ss << "\t\t\tthrow new RuntimeException(\"Unsupported screen type '\" + "
		"getContext().getString(R.string.YIP_screen_type) + \"'.\");\n";
	ss << "\t}\n";
	ss << '\n';
	ss << "\t/*@Override*/ public boolean shouldDelayChildPressedState()\n";
	ss << "\t{\n";
	ss << "\t\treturn false;\n";
	ss << "\t}\n";
	ss << '\n';
	ss << "\t@Override public void onMeasure(int widthSpec, int heightSpec)\n";
	ss << "\t{\n";
	ss << "\t\tsetMeasuredDimension(MeasureSpec.getSize(widthSpec), MeasureSpec.getSize(heightSpec));\n";
	ss << "\t}\n";
	ss << '\n';
	ss << "\t@Override public void onLayout(boolean changed, int left, int top, int right, int bottom)\n";
	ss << "\t{\n";
	ss << "\t\tboolean landscape = (getResources().getConfiguration().orientation == "
		"Configuration.ORIENTATION_LANDSCAPE);\n";
	if (hasPhone)
	{
		ss << '\n';
		ss << "\t\tif (\"P\".equals(getContext().getString(R.string.YIP_screen_type)))\n";
		ss << "\t\t{\n";
		ss << "\t\t\tfinal float horzScale = (float)(right - left) / (float)(landscape ? "
			<< phoneLayout->landscapeWidth() << " : " << phoneLayout->width() << ");\n";
		ss << "\t\t\tfinal float vertScale = (float)(bottom - top) / (float)(landscape ? "
			<< phoneLayout->landscapeHeight() << " : " << phoneLayout->height() << ");\n";
		for (auto it : widgetInfos)
		{
			const UIWidgetPtr & w = it.second.phone;
			if (!w.get())
				continue;
			ss << '\n';
			w->androidGenerateLayoutCode("\t\t\t", ss);
		}
		ss << "\t\t}\n";
	}
	if (hasTablet7)
	{
		ss << '\n';
		ss << "\t\tif (\"7\".equals(getContext().getString(R.string.YIP_screen_type)))\n";
		ss << "\t\t{\n";
		ss << "\t\t\tfinal float horzScale = (float)(right - left) / (float)(landscape ? "
			<< tablet7Layout->landscapeWidth() << " : " << tablet7Layout->width() << ");\n";
		ss << "\t\t\tfinal float vertScale = (float)(bottom - top) / (float)(landscape ? "
			<< tablet7Layout->landscapeHeight() << " : " << tablet7Layout->height() << ");\n";
		for (auto it : widgetInfos)
		{
			const UIWidgetPtr & w = it.second.tablet7;
			if (!w.get())
				continue;
			ss << '\n';
			w->androidGenerateLayoutCode("\t\t\t", ss);
		}
		ss << "\t\t}\n";
	}
	if (hasTablet10)
	{
		ss << '\n';
		ss << "\t\tif (\"T\".equals(getContext().getString(R.string.YIP_screen_type)))\n";
		ss << "\t\t{\n";
		ss << "\t\t\tfinal float horzScale = (float)(right - left) / (float)(landscape ? "
			<< tablet10Layout->landscapeWidth() << " : " << tablet10Layout->width() << ");\n";
		ss << "\t\t\tfinal float vertScale = (float)(bottom - top) / (float)(landscape ? "
			<< tablet10Layout->landscapeHeight() << " : " << tablet10Layout->height() << ");\n";
		for (auto it : widgetInfos)
		{
			const UIWidgetPtr & w = it.second.tablet10;
			if (!w.get())
				continue;
			ss << '\n';
			w->androidGenerateLayoutCode("\t\t\t", ss);
		}
		ss << "\t\t}\n";
	}
	ss << "\t}\n";
	ss << "}\n";

	project->yipDirectory()->writeFile(targetPath, ss.str());
}

void uiGenerateAndroidCommon(const ProjectPtr & project, const std::map<std::string, std::string> & translations)
{
	std::string yipDir = project->yipDirectory()->path();
	project->androidAddJavaSourceDir(pathConcat(yipDir, ".yip-android"));

	// Screen type

	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<resources>\n";
	ss << "\t<string name=\"YIP_screen_type\">P</string>\n";
	ss << "</resources>\n";
	project->yipDirectory()->writeFile("android/res/values/YIP_screen.xml", ss.str());

	ss.str(std::string());
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<resources>\n";
	ss << "\t<string name=\"YIP_screen_type\">7</string>\n";
	ss << "</resources>\n";
	project->yipDirectory()->writeFile("android/res/values-sw600dp/YIP_screen.xml", ss.str());

	ss.str(std::string());
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<resources>\n";
	ss << "\t<string name=\"YIP_screen_type\">T</string>\n";
	ss << "</resources>\n";
	project->yipDirectory()->writeFile("android/res/values-sw720dp/YIP_screen.xml", ss.str());

	// Translations

	ss.str(std::string());
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<resources>\n";
	for (const auto & it : translations)
	{
		ss << "\t<string name=\"" << xmlEscape(it.first) << "\" formatted=\"false\">";
		ss << xmlEscape(it.second);
		ss << "</string>\n";
	}
	ss << "</resources>\n";
	project->yipDirectory()->writeFile("android/res/values/YIP_translations.xml", ss.str());

	for (const auto & it : project->translationFiles())
	{
		ss.str(std::string());
		ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		ss << "<resources>\n";
		for (const auto & jt : translations)
		{
			ss << "\t<string name=\"" << xmlEscape(jt.first) << "\" formatted=\"false\">";
			ss << xmlEscape(it.second->getTranslation(jt.second));
			ss << "</string>\n";
		};
		ss << "</resources>\n";
		project->yipDirectory()->writeFile("android/res/values-" + it.first + "/YIP_translations.xml", ss.str());
	}
}
