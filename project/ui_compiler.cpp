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
#include "../util/path.h"
#include "../util/sha1.h"
#include <string>
#include <sstream>

static void generateIOSViewControllerH(const ProjectPtr & project, const Project::IOSViewController & cntrl)
{
	std::string yipDir = project->yipDirectory()->path();

	std::string targetName = cntrl.name;
	std::string targetPath = pathConcat(".yip-ios-view-controllers", targetName) + ".h";

	bool shouldProcessFile =
		(cntrl.iphonePortrait.get() &&
			project->yipDirectory()->shouldProcessFile(targetPath, cntrl.iphonePortrait->path())) ||
		(cntrl.iphoneLandscape.get() &&
			project->yipDirectory()->shouldProcessFile(targetPath, cntrl.iphoneLandscape->path())) ||
		(cntrl.ipadPortrait.get() &&
			project->yipDirectory()->shouldProcessFile(targetPath, cntrl.ipadPortrait->path())) ||
		(cntrl.ipadLandscape.get() &&
			project->yipDirectory()->shouldProcessFile(targetPath, cntrl.ipadLandscape->path()));
	if (!shouldProcessFile)
	{
		SourceFilePtr sourceFile = project->addSourceFile(targetPath, pathConcat(yipDir, targetPath));
		sourceFile->setIsGenerated(true);
		sourceFile->setPlatforms(Platform::iOS);
		return;
	}

	std::stringstream ss;
	ss << "#import <UIKit/UIKit.h>\n";
	ss << "@interface " << cntrl.name << " : " << cntrl.parentClass << "\n";
	ss << "@end\n";

	std::string generatedPath = project->yipDirectory()->writeFile(targetPath, ss.str());
	SourceFilePtr sourceFile = project->addSourceFile(targetPath, generatedPath);
	sourceFile->setIsGenerated(true);
	sourceFile->setPlatforms(Platform::iOS);
}

static void generateIOSViewControllerM(const ProjectPtr & project, const Project::IOSViewController & cntrl)
{
	std::string yipDir = project->yipDirectory()->path();

	std::string targetName = cntrl.name;
	std::string targetPath = pathConcat(".yip-ios-view-controllers", targetName) + ".m";

	bool shouldProcessFile =
		(cntrl.iphonePortrait.get() &&
			project->yipDirectory()->shouldProcessFile(targetPath, cntrl.iphonePortrait->path())) ||
		(cntrl.iphoneLandscape.get() &&
			project->yipDirectory()->shouldProcessFile(targetPath, cntrl.iphoneLandscape->path())) ||
		(cntrl.ipadPortrait.get() &&
			project->yipDirectory()->shouldProcessFile(targetPath, cntrl.ipadPortrait->path())) ||
		(cntrl.ipadLandscape.get() &&
			project->yipDirectory()->shouldProcessFile(targetPath, cntrl.ipadLandscape->path()));
	if (!shouldProcessFile)
	{
		SourceFilePtr sourceFile = project->addSourceFile(targetPath, pathConcat(yipDir, targetPath));
		sourceFile->setIsGenerated(true);
		sourceFile->setPlatforms(Platform::iOS);
		return;
	}

	std::stringstream ss;
	ss << "#import \"" << targetName << ".h\"\n";
	ss << "@implementation " << cntrl.name << '\n';
	ss << "@end\n";

	std::string generatedPath = project->yipDirectory()->writeFile(targetPath, ss.str());
	SourceFilePtr sourceFile = project->addSourceFile(targetPath, generatedPath);
	sourceFile->setIsGenerated(true);
	sourceFile->setPlatforms(Platform::iOS);
}

void compileUI(const ProjectPtr & project)
{
	for (const Project::IOSViewController & cntrl : project->iosViewControllers())
	{
		generateIOSViewControllerH(project, cntrl);
		generateIOSViewControllerM(project, cntrl);
	}
}
