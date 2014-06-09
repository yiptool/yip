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
#include "ios_generator.h"
#include "android_generator.h"
#include "parse_util.h"

static void uiCompileIOS(const ProjectPtr & project)
{
	UILayoutMap layouts;

	for (const Project::IOSViewController & cntrl : project->iosViewControllers())
		uiGenerateIOSViewController(layouts, project, cntrl);
}

static void uiCompileAndroid(const ProjectPtr & project)
{
	std::map<std::string, std::string> translations;
	UILayoutMap layouts;

	if (!project->androidViews().empty())
	{
		for (const Project::AndroidView & view : project->androidViews())
			uiGenerateAndroidView(layouts, project, view, translations);
		uiGenerateAndroidCommon(project, translations);
	}
}

void compileUI(const ProjectPtr & project)
{
	uiCompileIOS(project);
	uiCompileAndroid(project);
}
