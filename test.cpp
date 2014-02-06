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
#include "xcode_project.h"
#include <cstdio>
#include <cstring>
#include <cerrno>

int main()
{
	XCodeProject project;

	project.setUniqueID("A38BD24118A3044000AD1F3F");
	project.setOrganizationName("FriedCroc");

	XCodeGroup * mainGroup = project.addGroup();
	project.setMainGroup(mainGroup);
	mainGroup->setUniqueID("A38BD24018A3044000AD1F3F");

	XCodeGroup * productsGroup = project.addGroup();
	project.setProductRefGroup(productsGroup);
	productsGroup->setName("Products");
	productsGroup->setUniqueID("A38BD24A18A3044000AD1F3F");

	XCodeGroup * frameworksGroup = project.addGroup();
	frameworksGroup->setName("Frameworks");
	frameworksGroup->setUniqueID("A38BD24B18A3044000AD1F3F");

	XCodeGroup * sourcesGroup = project.addGroup();
	sourcesGroup->setPath("PRODPROD");
	sourcesGroup->setUniqueID("A38BD25218A3044000AD1F3F");

	XCodeGroup * supportingFilesGroup = project.addGroup();
	supportingFilesGroup->setName("Supporting Files");
	supportingFilesGroup->setUniqueID("A38BD25318A3044000AD1F3F");

	mainGroup->addChild(sourcesGroup);
	mainGroup->addChild(frameworksGroup);
	mainGroup->addChild(productsGroup);

	sourcesGroup->addChild(supportingFilesGroup);

	std::string data = project.toString();
	FILE * f = fopen("project.pbxproj", "wb");
	if (!f)
	{
		fprintf(stderr, "Unable to create file 'project.pbxproj': %s\n", strerror(errno));
		return 1;
	}
	fwrite(data.c_str(), 1, data.length(), f);
	fclose(f);

	return 0;
}
