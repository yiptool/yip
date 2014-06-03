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
#include "widgets/ui_switch.h"
#include "../util/cxx-util/cxx-util/fmt.h"
#include "../util/cxx-util/cxx-util/replace.h"
#include "../util/path-util/path-util.h"
#include <cassert>
#include <stdexcept>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uiGenerateAndroidView(UILayoutMap & layouts, const ProjectPtr & project,
	const Project::AndroidView & view)
{
	std::string yipDir = project->yipDirectory()->path();

	std::string targetName = replace(view.name, '.', "/");
	std::string targetPath = pathConcat(".yip-android", targetName) + ".java";

	bool shouldProcessFile =
		(view.phone.get() &&
			project->yipDirectory()->shouldProcessFile(targetPath, view.phone->path(), true)) ||
		(view.tablet7.get() &&
			project->yipDirectory()->shouldProcessFile(targetPath, view.tablet7->path(), true)) ||
		(view.tablet10.get() &&
			project->yipDirectory()->shouldProcessFile(targetPath, view.tablet10->path(), true));

	if (!shouldProcessFile)
	{
		for (auto it : project->translationFiles())
		{
			if (project->yipDirectory()->shouldProcessFile(targetPath, it.second->path(), false))
			{
				shouldProcessFile = true;
				break;
			}
		}
	}

	if (!shouldProcessFile)
		return;

	UILayoutPtr phoneLayout = uiLoadLayout(layouts, view.phone);
	UILayoutPtr tablet7Layout = uiLoadLayout(layouts, view.tablet7);
	UILayoutPtr tablet10Layout = uiLoadLayout(layouts, view.tablet10);

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
	ss << "import android.view.View;\n";
	ss << "import android.view.View.MeasureSpec;\n";
	ss << "import android.view.ViewGroup;\n";
	ss << "import android.util.AttributeSet;\n";
	ss << '\n';
	ss << "public class " << className << " extends ViewGroup\n";
	ss << "{\n";
	for (auto it : widgetInfos)
	{
		const UIWidgetPtr & widget = (it.second.phone.get() ? it.second.iphone :
			(it.second.tablet7.get() ? it.second.tablet7 : it.second.tablet10));
		ss << "public final " << widget->androidClassName() << ' ' << it.first << ";\n";
	}
	ss << '\n';
	ss << "\tpublic " << className << "(Context context)\n";
	ss << "\t{\n";
	ss << "\t\tsuper(context);\n";
	ss << "\t\tyip_createChildViews();\n";
	ss << "\t}\n";
	ss << '\n';
	ss << "\tpublic " << className << "(Context context, AttributeSet attrs)\n";
	ss << "\t{\n";
	ss << "\t\tsuper(context, attrs, 0);\n";
	ss << "\t\tyip_createChildViews();\n";
	ss << "\t}\n";
	ss << '\n';
	ss << "\tpublic " << className << "(Context context, AttributeSet attrs, int defStyle)\n";
	ss << "\t{\n";
	ss << "\t\tsuper(context, attrs, defStyle);\n";
	ss << "\t\tyip_createChildViews();\n";
	ss << "\t}\n";
	ss << '\n';
	ss << "\t@Override public boolean shouldDelayChildPressedState()\n";
	ss << "\t{\n";
	ss << "\t\treturn false;\n";
	ss << "\t}\n";
	ss << '\n';
	ss << "\t@Override public void onMeasure(int widthSpec, int heightSpec)\n";
	ss << "\t{\n";
	ss << "\t\tsetMeasuredDimension(MeasureSpec.getSize(widthSpec), MeasureSpec.getSize(heightSpec));\n";
	ss << "\t}\n";
	ss << '\n';
	ss << "\tprivate final void yip_createChildViews()\n";
	ss << "\t{\n";
	// FIXME
	ss << "\t}\n";
	ss << '\n';
	ss << "\t@Override public void onLayout(boolean changed, int left, int top, int right, int bottom)\n";
	ss << "\t{\n";
	// FIXME
	ss << "\t}\n";
	ss << "}\n";

	project->yipDirectory()->writeFile(targetPath, ss.str());
}

void uiGenerateAndroidCommon(const ProjectPtr & project)
{
	std::string yipDir = project->yipDirectory()->path();
	project->androidAddJavaSourceDir(pathConcat(yipDir, ".yip-android"));

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
}
