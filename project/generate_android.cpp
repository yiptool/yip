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
#include "generate_android.h"
#include "../util/xml.h"
#include "../util/path.h"
#include <map>
#include <sstream>
#include <cassert>
#include <memory>

namespace
{
	struct Gen
	{
		ProjectPtr project;
		std::string projectName;
		std::string projectPath;

		void writeIpr();
		void writeIml();
		void writeStringsXml();
		void writeApplicationMk();
		void writeAndroidMk();
		void writeDefaultProperties();
		void writeAndroidManifest();
		void generate();
	};
}

static bool isJavaFileType(FileType type)
{
	switch (type)
	{
	case FILE_SOURCE_JAVA:
		return true;
	default:
		return false;
	}
}

static bool isJNIFileType(FileType type)
{
	switch (type)
	{
	case FILE_SOURCE_C:
	case FILE_SOURCE_CXX:
		return true;
	default:
		return false;
	}
}

void Gen::writeIpr()
{
	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<project version=\"4\">\n";
	ss << "  <component name=\"CompilerConfiguration\">\n";
	ss << "    <option name=\"DEFAULT_COMPILER\" value=\"Javac\" />\n";
	ss << "    <resourceExtensions />\n";
	ss << "    <wildcardResourcePatterns>\n";
	ss << "      <entry name=\"!?*.java\" />\n";
	ss << "      <entry name=\"!?*.form\" />\n";
	ss << "      <entry name=\"!?*.class\" />\n";
	ss << "      <entry name=\"!?*.groovy\" />\n";
	ss << "      <entry name=\"!?*.scala\" />\n";
	ss << "      <entry name=\"!?*.flex\" />\n";
	ss << "      <entry name=\"!?*.kt\" />\n";
	ss << "      <entry name=\"!?*.clj\" />\n";
	ss << "    </wildcardResourcePatterns>\n";
	ss << "    <annotationProcessing>\n";
	ss << "      <profile default=\"true\" name=\"Default\" enabled=\"false\">\n";
	ss << "        <processorPath useClasspath=\"true\" />\n";
	ss << "      </profile>\n";
	ss << "    </annotationProcessing>\n";
	ss << "  </component>\n";
	ss << "  <component name=\"CopyrightManager\" default=\"\" />\n";
	ss << "  <component name=\"DependencyValidationManager\">\n";
	ss << "    <option name=\"SKIP_IMPORT_STATEMENTS\" value=\"false\" />\n";
	ss << "  </component>\n";
	ss << "  <component name=\"Encoding\" useUTFGuessing=\"true\" native2AsciiForPropertiesFiles=\"false\" />\n";
	ss << "  <component name=\"EntryPointsManager\">\n";
	ss << "    <entry_points version=\"2.0\" />\n";
	ss << "  </component>\n";
	ss << "  <component name=\"IdProvider\" IDEtalkID=\"C53185DCF1E514DDF8484167AF8648BB\" />\n";
	ss << "  <component name=\"ProjectModuleManager\">\n";
	ss << "    <modules>\n";
	ss << "      <module fileurl=\"file://$PROJECT_DIR$/" << xmlEscape(projectName) << ".iml\" "
					"filepath=\"$PROJECT_DIR$/" << xmlEscape(projectName) << ".iml\" />\n";
	ss << "    </modules>\n";
	ss << "  </component>\n";
	ss << "  <component name=\"ProjectRootManager\" version=\"2\" languageLevel=\"JDK_1_7\" "
					"assert-keyword=\"true\" jdk-15=\"true\" project-jdk-name=\"Android API 10 Platform\" "
					"project-jdk-type=\"Android SDK\">\n";
	ss << "    <output url=\"file://$PROJECT_DIR$/out\" />\n";
	ss << "  </component>\n";
	ss << "  <component name=\"VcsDirectoryMappings\">\n";
	ss << "    <mapping directory=\"\" vcs=\"\" />\n";
	ss << "  </component>\n";
	ss << "</project>\n";

	project->yipDirectory()->writeFile(projectName + "/" + projectName + ".ipr", ss.str());
}

void Gen::writeIml()
{
	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<module type=\"JAVA_MODULE\" version=\"4\">\n";
	ss << "  <component name=\"FacetManager\">\n";
	ss << "    <facet type=\"android\" name=\"Android\">\n";
	ss << "      <configuration />\n";
	ss << "    </facet>\n";
	ss << "  </component>\n";
	ss << "  <component name=\"NewModuleRootManager\" inherit-compiler-output=\"true\">\n";
	ss << "    <exclude-output />\n";
	ss << "    <content url=\"file://$MODULE_DIR$\">\n";
	ss << "      <sourceFolder url=\"file://$MODULE_DIR$/src\" isTestSource=\"false\" />\n";
	ss << "      <sourceFolder url=\"file://$MODULE_DIR$/gen\" isTestSource=\"false\" generated=\"true\" />\n";
	ss << "    </content>\n";
	ss << "    <orderEntry type=\"inheritedJdk\" />\n";
	ss << "    <orderEntry type=\"sourceFolder\" forTests=\"false\" />\n";
	ss << "  </component>\n";
	ss << "</module>\n";
	project->yipDirectory()->writeFile(projectName + "/" + projectName + ".iml", ss.str());
}

void Gen::writeStringsXml()
{
	std::stringstream ss;
	ss << "<resources>\n";
	ss << "\t<string name=\"app_title\">" << xmlEscape(project->androidDisplayName()) << "</string>\n";
	ss << "</resources>\n";
	project->yipDirectory()->writeFile(projectName + "/res/values/strings.xml", ss.str());
}

void Gen::writeApplicationMk()
{
	std::stringstream ss;
	ss << "APP_ABI := armeabi x86\n";		// armeabi-v7 mips
	project->yipDirectory()->writeFile(projectName + "/jni/Application.mk", ss.str());
}

void Gen::writeAndroidMk()
{
	std::stringstream ss;
	ss << "LOCAL_PATH := $(call my-dir)\n";
	ss << "include $(CLEAR_VARS)\n";
	ss << "LOCAL_MODULE := libcode\n";
	ss << "LOCAL_CFLAGS := \n";
//	ss << "LOCAL_SRC_FILES := gl_code.cpp\n";
	ss << "LOCAL_LDLIBS := -llog -lGLESv2\n";
	ss << "include $(BUILD_SHARED_LIBRARY)\n";
	project->yipDirectory()->writeFile(projectName + "/jni/Android.mk", ss.str());
}

void Gen::writeDefaultProperties()
{
	std::stringstream ss;
	ss << "target=" << project->androidTarget() << '\n';
	project->yipDirectory()->writeFile(projectName + "/default.properties", ss.str());
}

void Gen::writeAndroidManifest()
{
	std::stringstream ss;
	ss << "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\" ";
	ss << "\t\tpackage=\"" << xmlEscape(project->androidPackage()) << "\">\n";
	ss << "\t<application\n";
	ss << "\t\t\tandroid:label=\"@string/app_title\">\n";

	for (const std::string & activity : project->androidActivities())
		ss << activity << '\n';

	ss << "\t</application>\n";

	if (!project->androidGlEsVersion().length())
	{
		ss << "\t<uses-feature android:glEsVersion=\"" << xmlEscape(project->androidGlEsVersion())
			<< "\" android:required=\"true\" />\n";
	}

	if (project->androidMinSdkVersion() > 0 || project->androidTargetSdkVersion() > 0)
	{
		ss << "\t<uses-sdk ";
		if (project->androidMinSdkVersion() > 0)
			ss << "android:minSdkVersion=\"" << project->androidMinSdkVersion() << "\" ";
		if (project->androidTargetSdkVersion() > 0)
			ss << "android:targetSdkVersion=\"" << project->androidTargetSdkVersion() << "\" ";
		ss << ">\n";
	}

	ss << "</manifest>\n";

	project->yipDirectory()->writeFile(projectName + "/AndroidManifest.xml", ss.str());
}

void Gen::generate()
{
	projectName = "android";
	projectPath = pathConcat(project->yipDirectory()->path(), projectName);

	writeIpr();
	writeIml();
	writeStringsXml();
	writeApplicationMk();
	writeAndroidMk();
	writeDefaultProperties();
	writeAndroidManifest();
}

std::string generateAndroid(const ProjectPtr & project)
{
	Gen gen;
	gen.project = project;
	gen.generate();
	return gen.projectPath;
}
