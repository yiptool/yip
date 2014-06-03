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
#include "../util/cxx-util/cxx-util/replace.h"
#include "../util/xml.h"
#include "../util/path-util/path-util.h"
#include <map>
#include <unordered_set>
#include <sstream>
#include <cassert>
#include <iostream>
#include <memory>

namespace
{
	struct Gen
	{
		ProjectPtr project;
		std::string projectName;
		std::string projectPath;
		std::unordered_set<std::string> assetFiles;
		std::unordered_set<std::string> srcFiles;

		void generateSrcFiles();
		void deleteOldSrcFiles(const std::string & path, const std::string & fullpath);
		void generateAssetFiles();
		void deleteOldAssetFiles(const std::string & path, const std::string & fullpath);
		void writeIpr();
		void writeIml();
		void writeIws();
		void writeMainActivityJava();
		void writeLogCxx();
		void writeStringsXml();
		void writeApplicationMk();
		void writeAndroidMk();
		void writeDefaultProperties();
		void writeAntProperties();
		void writeCustomRulesXml();
		void writeNdkBuildXml();
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

void Gen::generateSrcFiles()
{
	std::string srcDir = "android/src";
	for (auto it : project->sourceFiles())
	{
		const SourceFilePtr & file = it.second;

		if (!(file->platforms() & Platform::Android))
			continue;
		if (!isJavaFileType(file->type()))
			continue;

		std::string name = pathConcat(srcDir, file->name());
		std::string path = pathSimplify(pathConcat(project->yipDirectory()->path(), name));

		pathCreate(pathGetDirectory(path));
		pathCreateSymLink(file->path(), path);

		srcFiles.insert(pathToUnixSeparators(path));
	}
}

void Gen::deleteOldSrcFiles(const std::string & path, const std::string & fullpath)
{
	DirEntryList list = pathEnumDirectoryContents(fullpath);
	for (auto it : list)
	{
		std::string file = pathConcat(fullpath, it.name);

		if (it.type == DirEntry_Directory)
		{
			deleteOldSrcFiles(pathConcat(path, it.name), file);
			continue;
		}

		if (srcFiles.find(file) == srcFiles.end())
		{
			std::cout << "killing " << pathConcat(path, it.name).c_str() << std::endl;
			pathDeleteFile(file.c_str());
		}
	}
}

void Gen::generateAssetFiles()
{
	std::string assetDir = "android/assets";
	for (auto it : project->resourceFiles())
	{
		const SourceFilePtr & file = it.second;

		if (!(file->platforms() & Platform::Android))
			continue;

		std::string name = pathConcat(assetDir, file->name());
		std::string path = pathSimplify(pathConcat(project->yipDirectory()->path(), name));

		pathCreate(pathGetDirectory(path));
		pathCreateSymLink(file->path(), path);

		assetFiles.insert(pathToUnixSeparators(path));
	}
}

void Gen::deleteOldAssetFiles(const std::string & path, const std::string & fullpath)
{
	DirEntryList list = pathEnumDirectoryContents(fullpath);
	for (auto it : list)
	{
		std::string file = pathConcat(fullpath, it.name);

		if (it.type == DirEntry_Directory)
		{
			deleteOldAssetFiles(pathConcat(path, it.name), file);
			continue;
		}

		if (assetFiles.find(file) == assetFiles.end())
		{
			std::cout << "killing " << pathConcat(path, it.name).c_str() << std::endl;
			pathDeleteFile(file.c_str());
		}
	}
}

void Gen::writeIpr()
{
	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<project version=\"4\">\n";
	ss << "  <component name=\"AntConfiguration\">\n";
	ss << "    <buildFile url=\"file://$PROJECT_DIR$/ndk_build.xml\" />\n";
	ss << "  </component>\n";
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
					"assert-keyword=\"true\" jdk-15=\"true\" project-jdk-name=\"Android SDK\" "
					"project-jdk-type=\"Android SDK\">\n";
	ss << "    <output url=\"file://$PROJECT_DIR$/out\" />\n";
	ss << "  </component>\n";
	ss << "  <component name=\"VcsDirectoryMappings\">\n";
	ss << "    <mapping directory=\"\" vcs=\"\" />\n";
	ss << "  </component>\n";
	ss << "  <component name=\"libraryTable\">\n";
	ss << "    <library name=\"android-support-v4\">\n";
	ss << "      <CLASSES>\n";
	// FIXME FIXME FIXME
	ss << "        <root url=\"jar:///android-sdk/sdk/extras/android/support/v4/android-support-v4.jar!/\" />\n";
	ss << "      </CLASSES>\n";
	ss << "      <JAVADOC />\n";
	ss << "      <SOURCES />\n";
	ss << "    </library>\n";
	ss << "  </component>\n";
	ss << "</project>\n";
	ss << '\n';

	project->yipDirectory()->writeFile("android/" + projectName + ".ipr", ss.str());
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
	for (const std::string & path : project->androidJavaSourceDirs())
	{
		std::string fullPath = pathMakeAbsolute(path);
		ss << "    <content url=\"file://" << xmlEscape(pathGetDirectory(fullPath)) << "\">\n";
		ss << "      <sourceFolder url=\"file://" << xmlEscape(fullPath) << "\" isTestSource=\"false\" />\n";
		ss << "    </content>\n";
	}
	ss << "    <content url=\"file://$MODULE_DIR$\">\n";
	ss << "      <sourceFolder url=\"file://$MODULE_DIR$/src\" isTestSource=\"false\" />\n";
	ss << "      <sourceFolder url=\"file://$MODULE_DIR$/gen-a\" isTestSource=\"false\" />\n";
	ss << "      <sourceFolder url=\"file://$MODULE_DIR$/gen\" isTestSource=\"false\" generated=\"true\" />\n";
	ss << "    </content>\n";
	ss << "    <orderEntry type=\"inheritedJdk\" />\n";
	ss << "    <orderEntry type=\"sourceFolder\" forTests=\"false\" />\n";
	ss << "    <orderEntry type=\"library\" name=\"android-support-v4\" level=\"project\" />\n";
	ss << "  </component>\n";
	ss << "</module>\n";
	ss << '\n';
	project->yipDirectory()->writeFile("android/" + projectName + ".iml", ss.str());
}

void Gen::writeIws()
{
	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<project version=\"4\">\n";
	ss << "  <component name=\"RunManager\" selected=\"Android Application." << xmlEscape(projectName) << "\">\n";
	ss << "    <configuration default=\"false\" name=\"" << xmlEscape(projectName) << "\" type=\"AndroidRunConfigurationType\" factoryName=\"Android Application\">\n";
	ss << "      <module name=\"" << xmlEscape(projectName) << "\" />\n";
	ss << "      <option name=\"ACTIVITY_CLASS\" value=\"\" />\n";
	ss << "      <option name=\"MODE\" value=\"default_activity\" />\n";
	ss << "      <option name=\"DEPLOY\" value=\"true\" />\n";
	ss << "      <option name=\"ARTIFACT_NAME\" value=\"\" />\n";
	ss << "      <option name=\"TARGET_SELECTION_MODE\" value=\"USB_DEVICE\" />\n";
	ss << "      <option name=\"USE_LAST_SELECTED_DEVICE\" value=\"false\" />\n";
	ss << "      <option name=\"PREFERRED_AVD\" value=\"\" />\n";
	ss << "      <option name=\"USE_COMMAND_LINE\" value=\"true\" />\n";
	ss << "      <option name=\"COMMAND_LINE\" value=\"\" />\n";
	ss << "      <option name=\"WIPE_USER_DATA\" value=\"false\" />\n";
	ss << "      <option name=\"DISABLE_BOOT_ANIMATION\" value=\"false\" />\n";
	ss << "      <option name=\"NETWORK_SPEED\" value=\"full\" />\n";
	ss << "      <option name=\"NETWORK_LATENCY\" value=\"none\" />\n";
	ss << "      <option name=\"CLEAR_LOGCAT\" value=\"false\" />\n";
	ss << "      <option name=\"SHOW_LOGCAT_AUTOMATICALLY\" value=\"true\" />\n";
	ss << "      <option name=\"FILTER_LOGCAT_AUTOMATICALLY\" value=\"true\" />\n";
	ss << "      <RunnerSettings RunnerId=\"AndroidDebugRunner\" />\n";
	ss << "      <ConfigurationWrapper RunnerId=\"AndroidDebugRunner\" />\n";
	ss << "      <method>\n";
	ss << "        <option name=\"AntTarget\" enabled=\"true\" antfile=\"file://$PROJECT_DIR$/ndk_build.xml\" target=\"ndk-build\" />\n";
	ss << "        <option name=\"Make\" enabled=\"true\" />\n";
	ss << "      </method>\n";
	ss << "    </configuration>\n";
	ss << "    <list size=\"1\">\n";
	ss << "      <item index=\"0\" class=\"java.lang.String\" itemvalue=\"Android Application." << xmlEscape(projectName) << "\" />\n";
	ss << "    </list>\n";
	ss << "  </component>\n";
	ss << "</project>\n";
	ss << '\n';
	project->yipDirectory()->writeFile("android/" + projectName + ".iws", ss.str());
}

void Gen::writeMainActivityJava()
{
	std::string package = project->androidPackage();

	for (const std::pair<std::string, std::string> & activity : project->androidMakeActivities())
	{
		std::stringstream ss;
		ss << "package " << package << ";\n";
		ss << "import android.content.res.AssetManager;\n";
		ss << "import android.os.Bundle;\n";
		ss << "public final class " << activity.first << " extends " << activity.second << " {\n";
		ss << "\tprivate static AssetManager assetManager;\n";
		ss << "\t@Override public void onCreate(Bundle savedInstanceState) {\n";
		ss << "\t\tassetManager = getAssets();\n";
		ss << "\t\tsetAssetManager(assetManager);\n";
		ss << "\t\tsuper.onCreate(savedInstanceState);\n";
		ss << "\t}\n";
		ss << "\tprivate static native void setAssetManager(AssetManager manager);\n";
		ss << "\tstatic { System.loadLibrary(\"code\"); }\n";
		ss << "};\n";

		std::string file = "android/gen-a/" + replace(package, '.', "/") + '/' + activity.first + ".java";
		project->yipDirectory()->writeFile(file, ss.str());
	}
}

void Gen::writeLogCxx()
{
	std::stringstream ss;

	ss << "#include <jni.h>\n";
	ss << "#include <android/log.h>\n";
	ss << "#include <android/asset_manager.h>\n";
	ss << "#include <android/asset_manager_jni.h>\n";
	ss << "#include <iostream>\n";
	ss << "#include <vector>\n";
	ss << "#include <sstream>\n";
	ss << "#include <cassert>\n";

	ss << "namespace\n";
	ss << "{\n";
	ss << "\tclass StreamRedirector : public std::streambuf\n";
	ss << "\t{\n";
	ss << "\tpublic:\n";
	ss << "\t\tStreamRedirector(std::ostream & stream, android_LogPriority priority)\n";
	ss << "\t\t\t: m_OriginalStream(stream),\n";
	ss << "\t\t\t  m_Priority(priority)\n";
	ss << "\t\t{\n";
	ss << "\t\t\tm_OriginalBuf = stream.rdbuf(this);\n";
	ss << "\t\t}\n";
	ss << "\t\t~StreamRedirector()\n";
	ss << "\t\t{\n";
	ss << "\t\t\tm_OriginalStream.rdbuf(m_OriginalBuf);\n";
	ss << "\t\t}\n";
	ss << "\tprotected:\n";
	ss << "\t\tint_type overflow(int_type ch) override\n";
	ss << "\t\t{\n";
	ss << "\t\t\tif (ch != '\\n' && ch != traits_type::eof())\n";
	ss << "\t\t\t\tm_Buffer << char(ch);\n";
	ss << "\t\t\telse\n";
	ss << "\t\t\t{\n";
	ss << "\t\t\t\tstd::string str = m_Buffer.str();\n";
	ss << "\t\t\t\t__android_log_print(m_Priority, \"C++\", \"%.*s\\n\", int(str.length()), str.data());\n";
	ss << "\t\t\t\tm_Buffer.str(std::string());\n";
	ss << "\t\t\t}\n";
	ss << "\t\t\treturn ch;\n";
	ss << "\t\t}\n";
	ss << "\tprivate:\n";
	ss << "\t\tstd::ostream & m_OriginalStream;\n";
	ss << "\t\tstd::streambuf * m_OriginalBuf;\n";
	ss << "\t\tstd::stringstream m_Buffer;\n";
	ss << "\t\tandroid_LogPriority m_Priority;\n";
	ss << "\t\tStreamRedirector(const StreamRedirector &) = delete;\n";
	ss << "\t\tStreamRedirector & operator=(const StreamRedirector &) = delete;\n";
	ss << "\t};\n";
	ss << "}\n";
	ss << "StreamRedirector redir_clog(std::clog, ANDROID_LOG_INFO);\n";
	ss << "StreamRedirector redir_cout(std::cout, ANDROID_LOG_INFO);\n";
	ss << "StreamRedirector redir_cerr(std::cerr, ANDROID_LOG_ERROR);\n";

	ss << "namespace Yip {\n";
	ss << "extern AAssetManager * g_AssetManager_8a09e478cb;\n";
	ss << "}\n";

	ss << "extern \"C\" {\n";

	std::string package = project->androidPackage();
	for (const std::pair<std::string, std::string> & activity : project->androidMakeActivities())
	{
		std::string fullName = replace(replace(package + '.' + activity.first, '.', "_"), '-', "_");
		ss << "JNIEXPORT void JNICALL Java_" << fullName << "_setAssetManager(JNIEnv * env, jclass, jobject mgr)\n";
		ss << "{\n";
		ss << "\tif (!Yip::g_AssetManager_8a09e478cb)\n";
		ss << "\t\tYip::g_AssetManager_8a09e478cb = AAssetManager_fromJava(env, mgr);\n";
		ss << "}\n";
	}

	ss << "} // extern \"C\"\n";

	project->yipDirectory()->writeFile("android/jni/log.cpp", ss.str());
}

void Gen::writeStringsXml()
{
	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<resources>\n";
	ss << "\t<string name=\"app_title\">" << xmlEscape(project->androidDisplayName()) << "</string>\n";
	ss << "</resources>\n";
	project->yipDirectory()->writeFile("android/res/values/strings.xml", ss.str());
}

void Gen::writeApplicationMk()
{
	std::stringstream ss;
	ss << "NDK_TOOLCHAIN_VERSION := clang\n";
	ss << "APP_CPPFLAGS := -std=c++11 -frtti -fexceptions\n";
	ss << "APP_STL := gnustl_static\n";
	ss << "APP_ABI := armeabi-v7a x86\n";		// armeabi mips
	project->yipDirectory()->writeFile("android/jni/Application.mk", ss.str());
}

void Gen::writeAndroidMk()
{
	std::string yipDir = pathMakeAbsolute(project->yipDirectory()->path());
	std::string prjDir = pathMakeAbsolute(pathConcat(project->yipDirectory()->path(), "android"));

	std::stringstream ss;
	ss << "LOCAL_PATH := /.\n";
	ss << "include $(CLEAR_VARS)\n";
	ss << "LOCAL_MODULE := libcode\n";

	ss << "LOCAL_CFLAGS :=";
	ss << " \\\n\t-Wno-deprecated";
	ss << " \\\n\t\"-I" << pathConcat(yipDir, ".yip-import-proxies") << "\"";
	for (auto it : project->headerPaths())
	{
		const HeaderPathPtr & headerPath = it.second;
		if (!(headerPath->platforms() & Platform::Android))
			continue;
		ss << " \\\n\t\"-I" << headerPath->path() << "\"";
	}
	ss << " \\\n\t-D__ANDROID__";
	for (auto it : project->defines())
	{
		const DefinePtr & define = it.second;
		if (!(define->platforms() & Platform::Android) || !(define->buildTypes() & BuildType::Release))
			continue;
		ss << " \\\n\t-D" << define->name();
	}
	ss << '\n';

	ss << "LOCAL_SRC_FILES :=";
	ss << " \\\n\t" << pathMakeAbsolute(pathConcat(project->yipDirectory()->path(), "android/jni/log.cpp"));
	for (auto it : project->sourceFiles())
	{
		const SourceFilePtr & file = it.second;

		if (!(file->platforms() & Platform::Android))
			continue;
		if (!isJNIFileType(file->type()))
			continue;
		ss << " \\\n\t" << pathMakeAbsolute(file->path());
	}
	ss << '\n';

	ss << "LOCAL_LDLIBS :=";
	for (const std::string & lib : project->androidNativeLibs())
		ss << " -l" << lib;
	ss << '\n';

	ss << "include $(BUILD_SHARED_LIBRARY)\n";
	project->yipDirectory()->writeFile("android/jni/Android.mk", ss.str());
}

void Gen::writeDefaultProperties()
{
	std::stringstream ss;
	ss << "target=" << project->androidTarget() << '\n';
	project->yipDirectory()->writeFile("android/default.properties", ss.str());
}

void Gen::writeAntProperties()
{
	std::stringstream ss;
	if (!project->androidJavaSourceDirs().empty())
	{
		ss << "source.absolute.dir=";
		ss << pathMakeAbsolute(pathConcat(project->yipDirectory()->path(), "android/src"));
		ss << ':' << pathMakeAbsolute(pathConcat(project->yipDirectory()->path(), "android/gen-a"));
		for (const std::string & path : project->androidJavaSourceDirs())
			ss << ':' << path;
		ss << '\n';
	}
	project->yipDirectory()->writeFile("android/ant.properties", ss.str());
}

void Gen::writeCustomRulesXml()
{
	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<project name=\"custom_rules\" basedir=\".\">\n";
	ss << "\t<target name=\"-pre-build\">\n";
	ss << "\t\t<mkdir dir=\"${jar.libs.absolute.dir}\" />\n";
	ss << "\t\t<copy todir=\"${jar.libs.absolute.dir}\">\n";
	ss << "\t\t\t<fileset dir=\"${sdk.dir}/extras/android/support/v4\">\n";
	ss << "\t\t\t\t<include name=\"android-support-v4.jar\" />\n";
	ss << "\t\t\t</fileset>\n";
	ss << "\t\t</copy>\n";
	ss << "\t</target>\n";
	ss << "</project>\n";
	project->yipDirectory()->writeFile("android/custom_rules.xml", ss.str());
}

void Gen::writeNdkBuildXml()
{
	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<project name=\"ndk_build\" default=\"ndk-build\" basedir=\".\">\n";
	ss << "\t<target name=\"ndk-build\">\n";
	ss << "\t\t<exec executable=\"ndk-build\" failonerror=\"true\" />\n";
	ss << "\t</target>\n";
	ss << "</project>\n";
	project->yipDirectory()->writeFile("android/ndk_build.xml", ss.str());
}

void Gen::writeAndroidManifest()
{
	std::stringstream ss;
	ss << "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"\n";
	ss << "\t\tpackage=\"" << xmlEscape(project->androidPackage()) << "\">\n";
	ss << "\t<application\n";
	ss << "\t\t\tandroid:label=\"@string/app_title\">\n";

	for (const std::string & activity : project->androidManifestActivities())
		ss << activity << '\n';

	ss << "\t</application>\n";

	if (!project->androidGlEsVersion().empty())
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
		ss << "/>\n";
	}

	ss << "</manifest>\n";

	project->yipDirectory()->writeFile("android/AndroidManifest.xml", ss.str());
}

void Gen::generate()
{
	projectName = project->projectName();
	projectPath = pathConcat(project->yipDirectory()->path(), "android");

	pathCreate(pathConcat(projectPath, "assets"));
	pathCreate(pathConcat(projectPath, "src"));
	pathCreate(pathConcat(projectPath, "gen-a"));

	generateSrcFiles();
	deleteOldSrcFiles("src", pathConcat(projectPath, "src"));

	generateAssetFiles();
	deleteOldAssetFiles("assets", pathConcat(projectPath, "assets"));

	writeIpr();
	writeIml();
	writeIws();

	writeLogCxx();
	writeMainActivityJava();
	writeStringsXml();

	writeApplicationMk();
	writeAndroidMk();

	writeDefaultProperties();
	writeAntProperties();
	writeCustomRulesXml();
	writeNdkBuildXml();

	writeAndroidManifest();
}

std::string generateAndroid(const ProjectPtr & project)
{
	Gen gen;
	gen.project = project;
	gen.generate();
	return pathConcat(gen.projectPath, project->projectName() + ".ipr");
}
