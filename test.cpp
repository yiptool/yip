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

	// File references

	XCodeFileReference * ref_app = project.addFileReference();
	ref_app->setUniqueID("A38BD24918A3044000AD1F3F");
	ref_app->setExplicitFileType(FILETYPE_WRAPPER_APPLICATION);
	ref_app->setIncludeInIndex(false);
	ref_app->setPath("PRODPROD.app");
	ref_app->setSourceTree("BUILT_PRODUCTS_DIR");

	XCodeFileReference * ref_foundation = project.addFileReference();
	ref_foundation->setUniqueID("A38BD24C18A3044000AD1F3F");
	ref_foundation->setLastKnownFileType(FILETYPE_WRAPPER_FRAMEWORK);
	ref_foundation->setName("Foundation.framework");
	ref_foundation->setPath("System/Library/Frameworks/Foundation.framework");
	ref_foundation->setSourceTree("SDKROOT");

	XCodeFileReference * ref_coreGraphics = project.addFileReference();
	ref_coreGraphics->setUniqueID("A38BD24E18A3044000AD1F3F");
	ref_coreGraphics->setLastKnownFileType(FILETYPE_WRAPPER_FRAMEWORK);
	ref_coreGraphics->setName("CoreGraphics.framework");
	ref_coreGraphics->setPath("System/Library/Frameworks/CoreGraphics.framework");
	ref_coreGraphics->setSourceTree("SDKROOT");

	XCodeFileReference * ref_uikit = project.addFileReference();
	ref_uikit->setUniqueID("A38BD25018A3044000AD1F3F");
	ref_uikit->setLastKnownFileType(FILETYPE_WRAPPER_FRAMEWORK);
	ref_uikit->setName("UIKit.framework");
	ref_uikit->setPath("System/Library/Frameworks/UIKit.framework");
	ref_uikit->setSourceTree("SDKROOT");

	XCodeFileReference * ref_plist = project.addFileReference();
	ref_plist->setUniqueID("A38BD25418A3044000AD1F3F");
	ref_plist->setLastKnownFileType(FILETYPE_TEXT_PLIST_XML);
	ref_plist->setPath("PRODPROD-Info.plist");

	XCodeFileReference * ref_plist_en = project.addFileReference();
	ref_plist_en->setUniqueID("A38BD25618A3044000AD1F3F");
	ref_plist_en->setLastKnownFileType(FILETYPE_TEXT_PLIST_STRINGS);
	ref_plist_en->setName("en");
	ref_plist_en->setPath("en.lproj/InfoPlist.strings");

	XCodeFileReference * ref_main_m = project.addFileReference();
	ref_main_m->setUniqueID("A38BD25818A3044000AD1F3F");
	ref_main_m->setLastKnownFileType(FILETYPE_SOURCECODE_C_OBJC);
	ref_main_m->setPath("main.m");

	XCodeFileReference * ref_prefix_pch = project.addFileReference();
	ref_prefix_pch->setUniqueID("A38BD25A18A3044000AD1F3F");
	ref_prefix_pch->setLastKnownFileType(FILETYPE_SOURCECODE_C_H);
	ref_prefix_pch->setPath("PRODPROD-Prefix.pch");

	XCodeFileReference * ref_appdelegate_h = project.addFileReference();
	ref_appdelegate_h->setUniqueID("A38BD25B18A3044000AD1F3F");
	ref_appdelegate_h->setLastKnownFileType(FILETYPE_SOURCECODE_C_H);
	ref_appdelegate_h->setPath("AppDelegate.h");

	XCodeFileReference * ref_appdelegate_m = project.addFileReference();
	ref_appdelegate_m->setUniqueID("A38BD25C18A3044000AD1F3F");
	ref_appdelegate_m->setLastKnownFileType(FILETYPE_SOURCECODE_C_OBJC);
	ref_appdelegate_m->setPath("AppDelegate.m");

	XCodeFileReference * ref_images_xcassets = project.addFileReference();
	ref_images_xcassets->setUniqueID("A38BD25E18A3044000AD1F3F");
	ref_images_xcassets->setLastKnownFileType(FILETYPE_FOLDER_ASSETCATALOG);
	ref_images_xcassets->setPath("Images.xcassets");

	XCodeFileReference * ref_xctest = project.addFileReference();
	ref_xctest->setUniqueID("A38BD26518A3044000AD1F3F");
	ref_xctest->setLastKnownFileType(FILETYPE_WRAPPER_FRAMEWORK);
	ref_xctest->setName("XCTest.framework");
	ref_xctest->setPath("Library/Frameworks/XCTest.framework");
	ref_xctest->setSourceTree("DEVELOPER_DIR");

	// Variant groups

	XCodeVariantGroup * varGroup = project.addVariantGroup();
	varGroup->setUniqueID("A38BD25518A3044000AD1F3F");
	varGroup->setName("InfoPlist.strings");
	varGroup->addChild(ref_plist_en);

	// Groups

	XCodeGroup * mainGroup = project.addGroup();
	project.setMainGroup(mainGroup);
	mainGroup->setUniqueID("A38BD24018A3044000AD1F3F");

	XCodeGroup * productsGroup = project.addGroup();
	project.setProductRefGroup(productsGroup);
	productsGroup->setName("Products");
	productsGroup->setUniqueID("A38BD24A18A3044000AD1F3F");
	productsGroup->addChild(ref_app);

	XCodeGroup * frameworksGroup = project.addGroup();
	frameworksGroup->setName("Frameworks");
	frameworksGroup->setUniqueID("A38BD24B18A3044000AD1F3F");
	frameworksGroup->addChild(ref_foundation);
	frameworksGroup->addChild(ref_coreGraphics);
	frameworksGroup->addChild(ref_uikit);
	frameworksGroup->addChild(ref_xctest);

	XCodeGroup * sourcesGroup = project.addGroup();
	sourcesGroup->setPath("PRODPROD");
	sourcesGroup->setUniqueID("A38BD25218A3044000AD1F3F");
	sourcesGroup->addChild(ref_appdelegate_h);
	sourcesGroup->addChild(ref_appdelegate_m);
	sourcesGroup->addChild(ref_images_xcassets);

	XCodeGroup * supportingFilesGroup = project.addGroup();
	supportingFilesGroup->setName("Supporting Files");
	supportingFilesGroup->setUniqueID("A38BD25318A3044000AD1F3F");
	supportingFilesGroup->addChild(ref_plist);
	supportingFilesGroup->addChild(varGroup);
	supportingFilesGroup->addChild(ref_main_m);
	supportingFilesGroup->addChild(ref_prefix_pch);

	mainGroup->addChild(sourcesGroup);
	mainGroup->addChild(frameworksGroup);
	mainGroup->addChild(productsGroup);

	sourcesGroup->addChild(supportingFilesGroup);

	// Build configurations

	XCodeBuildConfiguration * cfgProjectDebug = project.addBuildConfiguration();
	cfgProjectDebug->setUniqueID("A38BD27318A3044000AD1F3F");
	cfgProjectDebug->setName("Debug");

	XCodeBuildConfiguration * cfgProjectRelease = project.addBuildConfiguration();
	cfgProjectRelease->setUniqueID("A38BD27418A3044000AD1F3F");
	cfgProjectRelease->setName("Release");

	XCodeBuildConfiguration * cfgTargetDebug = project.addBuildConfiguration();
	cfgTargetDebug->setUniqueID("A38BD27618A3044000AD1F3F");
	cfgTargetDebug->setName("Debug");

	XCodeBuildConfiguration * cfgTargetRelease = project.addBuildConfiguration();
	cfgTargetRelease->setUniqueID("A38BD27718A3044000AD1F3F");
	cfgTargetRelease->setName("Release");

	// Configuration lists

	XCodeConfigurationList * projectCfgList = project.addConfigurationList();
	projectCfgList->setUniqueID("A38BD24418A3044000AD1F3F");
	projectCfgList->setDefaultConfigurationName("Release");
	projectCfgList->addConfiguration(cfgProjectDebug);
	projectCfgList->addConfiguration(cfgProjectRelease);
	project.setBuildConfigurationList(projectCfgList);

	XCodeConfigurationList * targetCfgList = project.addConfigurationList();
	targetCfgList->setUniqueID("A38BD27518A3044000AD1F3F");
	targetCfgList->setDefaultConfigurationName("Release");
	targetCfgList->addConfiguration(cfgTargetDebug);
	targetCfgList->addConfiguration(cfgTargetRelease);

	// --- Generate

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
