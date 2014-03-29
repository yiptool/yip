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
#include "generate_tizen.h"
#include "../util/fmt.h"
#include "../util/xml.h"
#include "../util/sha1.h"
#include "../util/path.h"
#include <sstream>
#include <unordered_set>
#include <iostream>

namespace
{
	struct Gen
	{
		// Input
		ProjectPtr project;

		// Output
		std::string projectPath;

		// Private
		std::string projectName;
		std::unordered_set<std::string> srcFiles;

		/* Methods */

		void generateSrcFiles();
		void deleteOldSrcFiles(const std::string & path, const std::string & fullpath);

		void writeManifest();

		void writeProjectFile();
		void writeCProjectFile();

		void generate();
	};
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool isCompilableFileType(FileType type)
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

static bool isHeaderFileType(FileType type)
{
	switch (type)
	{
	case FILE_SOURCE_C_HEADER:
	case FILE_SOURCE_CXX_HEADER:
		return true;
	default:
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Gen::generateSrcFiles()
{
	std::string srcDir = pathConcat(projectName, "src");
	for (auto it : project->sourceFiles())
	{
		const SourceFilePtr & file = it.second;

		if (!(file->platforms() & Platform::Tizen))
			continue;
		if (!isCompilableFileType(file->type()) && !isHeaderFileType(file->type()))
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

void Gen::writeManifest()
{
	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";
	ss << "<Manifest xmlns=\"http://schemas.tizen.org/2012/12/manifest\">\n";
	ss << "\t<Id>" << sha1(projectName).substr(0, 10) << "</Id>\n";
	ss << "\t<Version>2.2.0</Version>\n";
	ss << "\t<Type>C++App</Type>\n";
	ss << "\t<Requirements>\n";
	ss << "\t\t<Feature Name=\"http://tizen.org/feature/screen.size.all\">true</Feature>\n";
	ss << "\t</Requirements>\n";
	ss << "\t<Apps>\n";
	ss << "\t\t<ApiVersion>2.2</ApiVersion>\n";

	if (project->tizenPrivileges().empty())
		ss << "\t\t<Privileges />\n";
	else
	{
		ss << "\t\t<Privileges>\n";
		for (const std::string & privilege : project->tizenPrivileges())
			ss << "\t\t\t<Privilege>" << xmlEscape(privilege) << "</Privilege>\n";
		ss << "\t\t</Privileges>\n";
	}

	ss << "\t\t<UiApp Main=\"True\" MenuIconVisible=\"True\" Name=\"" << xmlEscape(projectName) << "\">\n";
	ss << "\t\t\t<UiScalability CoordinateSystem=\"Physical\" />\n";
	ss << "\t\t\t<UiTheme SystemTheme=\"Black\" />\n";
	ss << "\t\t\t<DisplayNames>\n";
	ss << "\t\t\t\t<DisplayName Locale=\"eng-GB\">" << xmlEscape(projectName) << "</DisplayName>\n";
	ss << "\t\t\t</DisplayNames>\n";
/*	FIXME
	ss << "\t\t\t<Icons>\n";
	ss << "\t\t\t\t<Icon Section=\"MainMenu\" Type=\"Xhigh\">mainmenu.png</Icon>\n";
	ss << "\t\t\t</Icons>\n";
*/
	ss << "\t\t\t<LaunchConditions />\n";
	ss << "\t\t\t<Notifications />\n";
	ss << "\t\t</UiApp>\n";
	ss << "\t</Apps>\n";
	ss << "</Manifest>\n";
	project->yipDirectory()->writeFile(pathConcat(projectName, "manifest.xml"), ss.str());
}

void Gen::writeProjectFile()
{
	std::string prjDir = pathMakeAbsolute(pathConcat(project->yipDirectory()->path(), projectName));

	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<projectDescription>\n";
	ss << "\t<name>" << xmlEscape(projectName) << "</name>\n";
	ss << "\t<comment></comment>\n";
	ss << "\t<projects>\n";
	ss << "\t</projects>\n";
	ss << "\t<buildSpec>\n";
	ss << "\t\t<buildCommand>\n";
	ss << "\t\t\t<name>org.eclipse.cdt.managedbuilder.core.genmakebuilder</name>\n";
	ss << "\t\t\t<arguments>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>?name?</key>\n";
	ss << "\t\t\t\t\t<value></value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>org.eclipse.cdt.make.core.append_environment</key>\n";
	ss << "\t\t\t\t\t<value>true</value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>org.eclipse.cdt.make.core.autoBuildTarget</key>\n";
	ss << "\t\t\t\t\t<value>all</value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>org.eclipse.cdt.make.core.buildArguments</key>\n";
	ss << "\t\t\t\t\t<value></value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>org.eclipse.cdt.make.core.buildCommand</key>\n";
	ss << "\t\t\t\t\t<value>make</value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>org.eclipse.cdt.make.core.buildLocation</key>\n";
	ss << "\t\t\t\t\t<value>" << xmlEscape(pathConcat(prjDir, "Debug")) << "</value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>org.eclipse.cdt.make.core.cleanBuildTarget</key>\n";
	ss << "\t\t\t\t\t<value>clean</value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>org.eclipse.cdt.make.core.contents</key>\n";
	ss << "\t\t\t\t\t<value>org.eclipse.cdt.make.core.activeConfigSettings</value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>org.eclipse.cdt.make.core.enableAutoBuild</key>\n";
	ss << "\t\t\t\t\t<value>true</value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>org.eclipse.cdt.make.core.enableCleanBuild</key>\n";
	ss << "\t\t\t\t\t<value>true</value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>org.eclipse.cdt.make.core.enableFullBuild</key>\n";
	ss << "\t\t\t\t\t<value>true</value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>org.eclipse.cdt.make.core.fullBuildTarget</key>\n";
	ss << "\t\t\t\t\t<value>all</value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>org.eclipse.cdt.make.core.stopOnError</key>\n";
	ss << "\t\t\t\t\t<value>true</value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t\t<dictionary>\n";
	ss << "\t\t\t\t\t<key>org.eclipse.cdt.make.core.useDefaultBuildCmd</key>\n";
	ss << "\t\t\t\t\t<value>true</value>\n";
	ss << "\t\t\t\t</dictionary>\n";
	ss << "\t\t\t</arguments>\n";
	ss << "\t\t</buildCommand>\n";
	ss << "\t\t<buildCommand>\n";
	ss << "\t\t\t<name>org.eclipse.cdt.managedbuilder.core.ScannerConfigBuilder</name>\n";
	ss << "\t\t\t<triggers>full,incremental,</triggers>\n";
	ss << "\t\t\t<arguments>\n";
	ss << "\t\t\t</arguments>\n";
	ss << "\t\t</buildCommand>\n";
	ss << "\t\t<buildCommand>\n";
	ss << "\t\t\t<name>org.tizen.nativecpp.apichecker.core.builder</name>\n";
	ss << "\t\t\t<arguments>\n";
	ss << "\t\t\t</arguments>\n";
	ss << "\t\t</buildCommand>\n";
	ss << "\t</buildSpec>\n";
	ss << "\t<natures>\n";
	ss << "\t\t<nature>org.eclipse.cdt.core.cnature</nature>\n";
	ss << "\t\t<nature>org.eclipse.cdt.core.ccnature</nature>\n";
	ss << "\t\t<nature>org.eclipse.cdt.managedbuilder.core.managedBuildNature</nature>\n";
	ss << "\t\t<nature>org.eclipse.cdt.managedbuilder.core.ScannerConfigNature</nature>\n";
	ss << "\t\t<nature>org.tizen.nativecpp.apichecker.core.tizenCppNature</nature>\n";
	ss << "\t</natures>\n";
	ss << "\t<filteredResources>\n";
	ss << "\t\t<filter>\n";
	ss << "\t\t\t<id>1395435667563</id>\n";
	ss << "\t\t\t<name></name>\n";
	ss << "\t\t\t<type>26</type>\n";
	ss << "\t\t\t<matcher>\n";
	ss << "\t\t\t\t<id>org.eclipse.ui.ide.multiFilter</id>\n";
	ss << "\t\t\t\t<arguments>1.0-projectRelativePath-matches-false-false-*/.tpk</arguments>\n";
	ss << "\t\t\t</matcher>\n";
	ss << "\t\t</filter>\n";
	ss << "\t</filteredResources>\n";
	ss << "</projectDescription>\n";
	project->yipDirectory()->writeFile(pathConcat(projectName, ".project"), ss.str());
}

void Gen::writeCProjectFile()
{
	std::string yipDir = pathMakeAbsolute(project->yipDirectory()->path());
	std::string prjDir = pathMakeAbsolute(pathConcat(project->yipDirectory()->path(), projectName));

	std::string headerPaths;
	{
		std::stringstream ss;
		ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\""
			<< xmlEscape(pathConcat(prjDir, "src")) << "\"/>\n";
		ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\""
			<< xmlEscape(yipDir) << "\"/>\n";
		ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\""
			<< xmlEscape(pathConcat(yipDir, ".yip-import-proxies")) << "\"/>\n";

		for (auto it : project->headerPaths())
		{
			const HeaderPathPtr & headerPath = it.second;
			if (!(headerPath->platforms() & Platform::Tizen))
				continue;
			ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\""
				<< xmlEscape(headerPath->path()) << "\"/>\n";
		}

		headerPaths = ss.str();
	}

	std::string debugDefines, releaseDefines;
	{
		std::stringstream ssD, ssR;
		ssD << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"__TIZEN__\"/>\n";
		ssR << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"__TIZEN__\"/>\n";
		for (auto it : project->defines())
		{
			const DefinePtr & define = it.second;
			if (!(define->platforms() & Platform::Tizen))
				continue;

			std::string name = xmlEscape(define->name());
			if (define->buildTypes() & BuildType::Debug)
				ssD << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"" << name << "\"/>\n";
			if (define->buildTypes() & BuildType::Release)
				ssR << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"" << name << "\"/>\n";
		}

		debugDefines = ssD.str();
		releaseDefines = ssR.str();
	}

	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
	ss << "<?fileVersion 4.0.0?>\n";
	ss << "\n";
	ss << "<cproject storage_type_id=\"org.eclipse.cdt.core.XmlProjectDescriptionStorage\">\n";
	ss << "\t<storageModule moduleId=\"org.eclipse.cdt.core.settings\">\n";
	ss << "\t\t<cconfiguration id=\"org.tizen.nativecpp.config.sbi.gcc45.app.debug.1450051639\">\n";
	ss << "\t\t\t<storageModule buildSystemId=\"org.eclipse.cdt.managedbuilder.core.configurationDataProvider\" "
			"id=\"org.tizen.nativecpp.config.sbi.gcc45.app.debug.1450051639\" "
			"moduleId=\"org.eclipse.cdt.core.settings\" name=\"Debug\">\n";
	ss << "\t\t\t\t<externalSettings/>\n";
	ss << "\t\t\t\t<extensions>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.ELF\" point=\"org.eclipse.cdt.core.BinaryParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.GmakeErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.CWDLocator\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.MakeErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.GCCErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.GASErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.GLDErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t</extensions>\n";
	ss << "\t\t\t</storageModule>\n";
	ss << "\t\t\t<storageModule moduleId=\"cdtBuildSystem\" version=\"4.0.0\">\n";
	ss << "\t\t\t\t<configuration artifactExtension=\"exe\" artifactName=\"${ProjName}\" "
			"buildArtefactType=\"org.tizen.nativecpp.buildArtefactType.app\" "
			"buildProperties=\"org.eclipse.cdt.build.core.buildType=org.eclipse.cdt.build.core.buildType.debug,"
				"org.eclipse.cdt.build.core.buildArtefactType=org.tizen.nativecpp.buildArtefactType.app\" "
			"description=\"\" "
			"errorParsers=\"org.eclipse.cdt.core.MakeErrorParser;org.eclipse.cdt.core.GCCErrorParser;\" "
			"id=\"org.tizen.nativecpp.config.sbi.gcc45.app.debug.1450051639\" "
			"name=\"Debug\" "
			"parent=\"org.tizen.nativecpp.config.sbi.gcc45.app.debug\">\n";
	ss << "\t\t\t\t\t<folderInfo id=\"org.tizen.nativecpp.config.sbi.gcc45.app.debug.1450051639.\" "
			"name=\"/\" resourcePath=\"\">\n";
	ss << "\t\t\t\t\t\t<toolChain id=\"org.tizen.nativecpp.toolchain.sbi.gcc45.app.debug.778907759\""
			" name=\"Tizen Native Toolchain\" superClass=\"org.tizen.nativecpp.toolchain.sbi.gcc45.app.debug\">\n";
	ss << "\t\t\t\t\t\t\t<targetPlatform binaryParser=\"org.eclipse.cdt.core.ELF\" "
			"id=\"org.tizen.nativeide.target.sbi.gnu.platform.base.480719475\" osList=\"linux,win32\" "
			"superClass=\"org.tizen.nativeide.target.sbi.gnu.platform.base\"/>\n";
	ss << "\t\t\t\t\t\t\t<builder autoBuildTarget=\"all\" buildPath=\"" << xmlEscape(pathConcat(prjDir, "Debug"))
			<< "\" enableAutoBuild=\"true\" id=\"org.tizen.nativecpp.target.sbi.gnu.builder.2136560509\" "
			"keepEnvironmentInBuildfile=\"false\" managedBuildOn=\"true\" name=\"Tizen Application Builder\" "
			"superClass=\"org.tizen.nativecpp.target.sbi.gnu.builder\"/>\n";
	ss << "\t\t\t\t\t\t\t<tool id=\"org.tizen.nativecpp.tool.sbi.gnu.archiver.1074060562\" name=\"Archiver\" "
			"superClass=\"org.tizen.nativecpp.tool.sbi.gnu.archiver\"/>\n";
	ss << "\t\t\t\t\t\t\t<tool command=\"clang++\" id=\"org.tizen.nativecpp.tool.sbi.gnu.cpp.compiler.162100078\" "
			"name=\"C++ Compiler\" superClass=\"org.tizen.nativecpp.tool.sbi.gnu.cpp.compiler\">\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.compiler.option.optimization.level.1005235375\" "
			"name=\"Optimization Level\" superClass=\"gnu.cpp.compiler.option.optimization.level\" "
			"value=\"gnu.cpp.compiler.optimization.level.none\" valueType=\"enumerated\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.debugging.level.805291016\" "
			"name=\"Debug level\" superClass=\"sbi.gnu.cpp.compiler.option.debugging.level\" "
			"value=\"gnu.cpp.compiler.debugging.level.max\" valueType=\"enumerated\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.debug.applog.1712356008\" "
			"name=\"Enable application logging (-D_APP_LOG)\" "
			"superClass=\"sbi.gnu.cpp.compiler.option.debug.applog\" value=\"true\" valueType=\"boolean\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.1008838958\" name=\"Tizen-Target\" "
			"superClass=\"sbi.gnu.cpp.compiler.option\" valueType=\"userObjs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"tizen-device-2.2.native_llvm31.armel.cpp.app\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.frameworks_inc.cpp.1240874537\" "
			"name=\"Tizen-Frameworks-Include-Path\" superClass=\"sbi.gnu.cpp.compiler.option.frameworks_inc.cpp\" "
			"valueType=\"includePath\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include/libxml2&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include/osp&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.frameworks_cflags.cpp.105921041\" "
			"name=\"Tizen-Frameworks-Other-Cflags\" "
			"superClass=\"sbi.gnu.cpp.compiler.option.frameworks_cflags.cpp\" valueType=\"stringList\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-target arm-tizen-linux-gnueabi "
			"-gcc-toolchain &quot;${SBI_SYSROOT}/../../../../tools/arm-linux-gnueabi-gcc-4.5/&quot; "
			"-ccc-gcc-name arm-linux-gnueabi-g++ -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 "
			"-mtune=cortex-a8 -Wno-gnu\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\" -fPIE\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"--sysroot=&quot;${SBI_SYSROOT}&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.compiler.option.include.paths.1021410165\" "
			"name=\"Include paths (-I)\" superClass=\"gnu.cpp.compiler.option.include.paths\" "
			"valueType=\"includePath\">\n";
	ss << headerPaths;
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.frameworks.cpp.663724219\" "
			"name=\"Tizen-Frameworks\" superClass=\"sbi.gnu.cpp.compiler.option.frameworks.cpp\" "
			"valueType=\"userObjs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"osp\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.compiler.option.preprocessor.def.1530115672\" "
			"name=\"Defined symbols (-D)\" superClass=\"gnu.cpp.compiler.option.preprocessor.def\" "
			"valueType=\"definedSymbols\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"_DEBUG\"/>\n";
	ss << debugDefines;
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.compiler.option.optimization.flags.267323937\" "
			"name=\"Other optimization flags\" superClass=\"gnu.cpp.compiler.option.optimization.flags\" "
			"value=\"-std=c++11\" valueType=\"string\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<inputType id=\"sbi.gnu.cpp.compiler.tizen.inputType.10051845\" "
			"superClass=\"sbi.gnu.cpp.compiler.tizen.inputType\"/>\n";
	ss << "\t\t\t\t\t\t\t</tool>\n";
	ss << "\t\t\t\t\t\t\t<tool command=\"clang\" id=\"org.tizen.nativecpp.tool.sbi.gnu.c.compiler.800151942\" "
			"name=\"C Compiler\" superClass=\"org.tizen.nativecpp.tool.sbi.gnu.c.compiler\">\n";
	ss << "\t\t\t\t\t\t\t\t<option defaultValue=\"gnu.c.optimization.level.none\" "
			"id=\"gnu.c.compiler.option.optimization.level.1421747840\" name=\"Optimization Level\" "
			"superClass=\"gnu.c.compiler.option.optimization.level\" valueType=\"enumerated\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.debugging.level.383781154\" "
			"name=\"Debug level\" superClass=\"sbi.gnu.c.compiler.option.debugging.level\" "
			"value=\"gnu.c.debugging.level.max\" valueType=\"enumerated\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.debug.applog.1622870369\" "
			"name=\"Enable application logging (-D_APP_LOG)\" "
			"superClass=\"sbi.gnu.c.compiler.option.debug.applog\" value=\"true\" valueType=\"boolean\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.1013462328\" "
			"name=\"Tizen-Target\" superClass=\"sbi.gnu.c.compiler.option\" valueType=\"userObjs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"tizen-device-2.2.native_llvm31.armel.cpp.app\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.frameworks_inc.cpp.1500860641\" "
			"name=\"Tizen-Frameworks-Include-Path\" superClass=\"sbi.gnu.c.compiler.option.frameworks_inc.cpp\" "
			"valueType=\"includePath\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include/libxml2&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include/osp&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.frameworks_cflags.cpp.223798636\" "
			"name=\"Tizen-Frameworks-Other-Cflags\" superClass=\"sbi.gnu.c.compiler.option.frameworks_cflags.cpp\" "
			"valueType=\"stringList\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-target arm-tizen-linux-gnueabi "
			"-gcc-toolchain &quot;${SBI_SYSROOT}/../../../../tools/arm-linux-gnueabi-gcc-4.5/&quot; "
			"-ccc-gcc-name arm-linux-gnueabi-g++ -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 "
			"-mtune=cortex-a8 -Wno-gnu\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\" -fPIE\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"--sysroot=&quot;${SBI_SYSROOT}&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.c.compiler.option.include.paths.1901595360\" "
			"name=\"Include paths (-I)\" superClass=\"gnu.c.compiler.option.include.paths\" "
			"valueType=\"includePath\">\n";
	ss << headerPaths;
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.frameworks.cpp.51354163\" "
			"name=\"Tizen-Frameworks\" superClass=\"sbi.gnu.c.compiler.option.frameworks.cpp\" "
			"valueType=\"userObjs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"osp\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.c.compiler.option.preprocessor.def.symbols.221505880\" "
			"name=\"Defined symbols (-D)\" superClass=\"gnu.c.compiler.option.preprocessor.def.symbols\" "
			"valueType=\"definedSymbols\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"_DEBUG\"/>\n";
	ss << debugDefines;
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<inputType id=\"sbi.gnu.c.compiler.tizen.inputType.273672733\" "
			"superClass=\"sbi.gnu.c.compiler.tizen.inputType\"/>\n";
	ss << "\t\t\t\t\t\t\t</tool>\n";
	ss << "\t\t\t\t\t\t\t<tool id=\"org.tizen.nativeide.tool.sbi.gnu.c.linker.base.1078788504\" "
			"name=\"C Linker\" superClass=\"org.tizen.nativeide.tool.sbi.gnu.c.linker.base\"/>\n";
	ss << "\t\t\t\t\t\t\t<tool command=\"clang++\" id=\"org.tizen.nativecpp.tool.sbi.gnu.cpp.linker.270107254\" "
			"name=\"C++ Linker\" superClass=\"org.tizen.nativecpp.tool.sbi.gnu.cpp.linker\">\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.linker.option.frameworks_lflags.cpp.2075295714\" "
			"name=\"Tizen-Frameworks-Other-Lflags\" superClass=\"sbi.gnu.cpp.linker.option.frameworks_lflags.cpp\" "
			"valueType=\"stringList\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-target arm-tizen-linux-gnueabi "
			"-gcc-toolchain &quot;${SBI_SYSROOT}/../../../../tools/arm-linux-gnueabi-gcc-4.5/&quot; "
			"-ccc-gcc-name arm-linux-gnueabi-g++ -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 "
			"-mtune=cortex-a8 -Xlinker --as-needed\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-pie -lpthread \"/>\n";
//	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-Xlinker "
//			"-rpath=&quot;/opt/usr/apps/ClJggHLRyf/lib&quot;\"/>\n";
//	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-Xlinker "
//			"-rpath=&quot;/home/developer/sdk_tools/lib&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"--sysroot=&quot;${SBI_SYSROOT}&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-L&quot;${SBI_SYSROOT}/usr/lib&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-L&quot;${SBI_SYSROOT}/usr/lib/osp&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-uifw\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-appfw\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-image\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-json\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-ime\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-net\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-lpthread\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-content\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-locations\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-telephony\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-uix\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-media\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-messaging\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-web\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-social\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-wifi\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-bluetooth\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-nfc\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-face\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-secure-element\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-speech-tts\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-shell\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-shell-core\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-speech-stt\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-vision\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-lxml2\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-lalut\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-lopenal\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.link.option.paths.631557974\" "
			"name=\"Library search path (-L)\" superClass=\"gnu.cpp.link.option.paths\" valueType=\"libPaths\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\""
			<< xmlEscape(pathConcat(prjDir, "lib")) << "\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.link.option.libs.1976314268\" name=\"Libraries (-l)\" "
			"superClass=\"gnu.cpp.link.option.libs\" valueType=\"libs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"rt\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<inputType id=\"cdt.managedbuild.tool.gnu.cpp.linker.input.827211201\" "
			"superClass=\"cdt.managedbuild.tool.gnu.cpp.linker.input\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<additionalInput kind=\"additionalinputdependency\" paths=\"$(USER_OBJS)\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<additionalInput kind=\"additionalinput\" paths=\"$(LIBS)\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</inputType>\n";
	ss << "\t\t\t\t\t\t\t</tool>\n";
	ss << "\t\t\t\t\t\t\t<tool command=\"arm-linux-gnueabi-as\" "
			"id=\"org.tizen.nativeapp.tool.sbi.gnu.assembler.base.1647526109\" name=\"Assembler\" "
			"superClass=\"org.tizen.nativeapp.tool.sbi.gnu.assembler.base\">\n";
	ss << "\t\t\t\t\t\t\t\t<inputType id=\"cdt.managedbuild.tool.gnu.assembler.input.1389854539\" "
			"superClass=\"cdt.managedbuild.tool.gnu.assembler.input\"/>\n";
	ss << "\t\t\t\t\t\t\t</tool>\n";
	ss << "\t\t\t\t\t\t\t<tool id=\"org.tizen.nativecpp.tool.sbi.po.compiler.147029023\" "
			"name=\"PO Resource Compiler\" superClass=\"org.tizen.nativecpp.tool.sbi.po.compiler\"/>\n";
	ss << "\t\t\t\t\t\t\t<tool id=\"org.tizen.nativecpp.tool.sbi.edc.compiler.1508161279\" "
			"name=\"EDC Resource Compiler\" superClass=\"org.tizen.nativecpp.tool.sbi.edc.compiler\"/>\n";
	ss << "\t\t\t\t\t\t</toolChain>\n";
	ss << "\t\t\t\t\t</folderInfo>\n";
	ss << "\t\t\t\t\t<sourceEntries>\n";
	ss << "\t\t\t\t\t\t<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"src\"/>\n";
	ss << "\t\t\t\t\t\t<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"inc\"/>\n";
	ss << "\t\t\t\t\t\t<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"res\"/>\n";
	ss << "\t\t\t\t\t</sourceEntries>\n";
	ss << "\t\t\t\t</configuration>\n";
	ss << "\t\t\t</storageModule>\n";
	ss << "\t\t\t<storageModule moduleId=\"org.eclipse.cdt.core.externalSettings\"/>\n";
	ss << "\t\t</cconfiguration>\n";
	ss << "\t\t<cconfiguration id=\"org.tizen.nativecpp.config.sbi.gcc45.app.release.598345466\">\n";
	ss << "\t\t\t<storageModule buildSystemId=\"org.eclipse.cdt.managedbuilder.core.configurationDataProvider\" "
			"id=\"org.tizen.nativecpp.config.sbi.gcc45.app.release.598345466\" "
			"moduleId=\"org.eclipse.cdt.core.settings\" name=\"Release\">\n";
	ss << "\t\t\t\t<externalSettings/>\n";
	ss << "\t\t\t\t<extensions>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.ELF\" point=\"org.eclipse.cdt.core.BinaryParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.GmakeErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.CWDLocator\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.MakeErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.GCCErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.GASErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.GLDErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t</extensions>\n";
	ss << "\t\t\t</storageModule>\n";
	ss << "\t\t\t<storageModule moduleId=\"cdtBuildSystem\" version=\"4.0.0\">\n";
	ss << "\t\t\t\t<configuration artifactExtension=\"exe\" artifactName=\"${ProjName}\" "
			"buildArtefactType=\"org.tizen.nativecpp.buildArtefactType.app\" "
			"buildProperties=\"org.eclipse.cdt.build.core.buildType="
				"org.eclipse.cdt.build.core.buildType.release,org.eclipse.cdt.build.core.buildArtefactType="
				"org.tizen.nativecpp.buildArtefactType.app\" "
			"description=\"\" "
			"errorParsers=\"org.eclipse.cdt.core.MakeErrorParser;org.eclipse.cdt.core.GCCErrorParser;\" "
			"id=\"org.tizen.nativecpp.config.sbi.gcc45.app.release.598345466\" name=\"Release\" "
			"parent=\"org.tizen.nativecpp.config.sbi.gcc45.app.release\">\n";
	ss << "\t\t\t\t\t<folderInfo id=\"org.tizen.nativecpp.config.sbi.gcc45.app.release.598345466.\" "
			"name=\"/\" resourcePath=\"\">\n";
	ss << "\t\t\t\t\t\t<toolChain id=\"org.tizen.nativecpp.toolchain.sbi.gcc45.app.release.1556373990\" "
			"name=\"Tizen Native Toolchain\" superClass=\"org.tizen.nativecpp.toolchain.sbi.gcc45.app.release\">\n";
	ss << "\t\t\t\t\t\t\t<targetPlatform binaryParser=\"org.eclipse.cdt.core.ELF\" "
			"id=\"org.tizen.nativeide.target.sbi.gnu.platform.base.682049264\" osList=\"linux,win32\" "
			"superClass=\"org.tizen.nativeide.target.sbi.gnu.platform.base\"/>\n";
	ss << "\t\t\t\t\t\t\t<builder buildPath=\"" << xmlEscape(pathConcat(prjDir, "Release")) << "\" "
			"id=\"org.tizen.nativecpp.target.sbi.gnu.builder.1284643935\" keepEnvironmentInBuildfile=\"false\" "
			"managedBuildOn=\"true\" name=\"Tizen Application Builder\" "
			"superClass=\"org.tizen.nativecpp.target.sbi.gnu.builder\"/>\n";
	ss << "\t\t\t\t\t\t\t<tool id=\"org.tizen.nativecpp.tool.sbi.gnu.archiver.74996715\" name=\"Archiver\" "
			"superClass=\"org.tizen.nativecpp.tool.sbi.gnu.archiver\"/>\n";
	ss << "\t\t\t\t\t\t\t<tool command=\"clang++\" id=\"org.tizen.nativecpp.tool.sbi.gnu.cpp.compiler.1919336427\" "
			"name=\"C++ Compiler\" superClass=\"org.tizen.nativecpp.tool.sbi.gnu.cpp.compiler\">\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.compiler.option.optimization.level.500484809\" "
			"name=\"Optimization Level\" superClass=\"gnu.cpp.compiler.option.optimization.level\" "
			"value=\"gnu.cpp.compiler.optimization.level.most\" valueType=\"enumerated\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.debugging.level.1047108025\" "
			"name=\"Debug level\" superClass=\"sbi.gnu.cpp.compiler.option.debugging.level\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.debug.applog.503884246\" "
			"name=\"Enable application logging (-D_APP_LOG)\" "
			"superClass=\"sbi.gnu.cpp.compiler.option.debug.applog\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.128755254\" "
			"name=\"Tizen-Target\" superClass=\"sbi.gnu.cpp.compiler.option\" valueType=\"userObjs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"tizen-device-2.2.native_llvm31.armel.cpp.app\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.frameworks_inc.cpp.497140623\" "
			"name=\"Tizen-Frameworks-Include-Path\" superClass=\"sbi.gnu.cpp.compiler.option.frameworks_inc.cpp\" "
			"valueType=\"includePath\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include/libxml2&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include/osp&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.frameworks_cflags.cpp.1112538490\" "
			"name=\"Tizen-Frameworks-Other-Cflags\" "
			"superClass=\"sbi.gnu.cpp.compiler.option.frameworks_cflags.cpp\" valueType=\"stringList\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-target arm-tizen-linux-gnueabi "
			"-gcc-toolchain &quot;${SBI_SYSROOT}/../../../../tools/arm-linux-gnueabi-gcc-4.5/&quot; "
			"-ccc-gcc-name arm-linux-gnueabi-g++ -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 "
			"-mtune=cortex-a8 -Wno-gnu\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\" -fPIE\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"--sysroot=&quot;${SBI_SYSROOT}&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.compiler.option.include.paths.1835549650\" "
			"name=\"Include paths (-I)\" superClass=\"gnu.cpp.compiler.option.include.paths\" "
			"valueType=\"includePath\">\n";
	ss << headerPaths;
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.frameworks.cpp.1494077313\" "
			"name=\"Tizen-Frameworks\" superClass=\"sbi.gnu.cpp.compiler.option.frameworks.cpp\" "
			"valueType=\"userObjs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"osp\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.compiler.option.optimization.flags.389214627\" "
			"name=\"Other optimization flags\" superClass=\"gnu.cpp.compiler.option.optimization.flags\" "
			"value=\"-std=c++11\" valueType=\"string\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.compiler.option.preprocessor.def.371022542\" "
			"superClass=\"gnu.cpp.compiler.option.preprocessor.def\" valueType=\"definedSymbols\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"NDEBUG\"/>\n";
	ss << releaseDefines;
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<inputType id=\"sbi.gnu.cpp.compiler.tizen.inputType.282478263\" "
			"superClass=\"sbi.gnu.cpp.compiler.tizen.inputType\"/>\n";
	ss << "\t\t\t\t\t\t\t</tool>\n";
	ss << "\t\t\t\t\t\t\t<tool command=\"clang\" id=\"org.tizen.nativecpp.tool.sbi.gnu.c.compiler.706995671\" "
			"name=\"C Compiler\" superClass=\"org.tizen.nativecpp.tool.sbi.gnu.c.compiler\">\n";
	ss << "\t\t\t\t\t\t\t\t<option defaultValue=\"gnu.c.optimization.level.most\" "
			"id=\"gnu.c.compiler.option.optimization.level.716123362\" name=\"Optimization Level\" "
			"superClass=\"gnu.c.compiler.option.optimization.level\" valueType=\"enumerated\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.debugging.level.1308738663\" "
			"name=\"Debug level\" superClass=\"sbi.gnu.c.compiler.option.debugging.level\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.debug.applog.1976451164\" "
			"name=\"Enable application logging (-D_APP_LOG)\" "
			"superClass=\"sbi.gnu.c.compiler.option.debug.applog\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.1612058185\" "
			"name=\"Tizen-Target\" superClass=\"sbi.gnu.c.compiler.option\" valueType=\"userObjs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"tizen-device-2.2.native_llvm31.armel.cpp.app\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.frameworks_inc.cpp.278486823\" "
			"name=\"Tizen-Frameworks-Include-Path\" superClass=\"sbi.gnu.c.compiler.option.frameworks_inc.cpp\" "
			"valueType=\"includePath\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include/libxml2&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include/osp&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.frameworks_cflags.cpp.2067551538\" "
			"name=\"Tizen-Frameworks-Other-Cflags\" superClass=\"sbi.gnu.c.compiler.option.frameworks_cflags.cpp\" "
			"valueType=\"stringList\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-target arm-tizen-linux-gnueabi "
			"-gcc-toolchain &quot;${SBI_SYSROOT}/../../../../tools/arm-linux-gnueabi-gcc-4.5/&quot; "
			"-ccc-gcc-name i386-linux-gnueabi-g++ -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 "
			"-mtune=cortex-a8 -Xlinker --as-needed\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\" -fPIE\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"--sysroot=&quot;${SBI_SYSROOT}&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.c.compiler.option.include.paths.1171951756\" "
			"name=\"Include paths (-I)\" superClass=\"gnu.c.compiler.option.include.paths\" "
			"valueType=\"includePath\">\n";
	ss << headerPaths;
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.frameworks.cpp.423016602\" "
			"name=\"Tizen-Frameworks\" superClass=\"sbi.gnu.c.compiler.option.frameworks.cpp\" "
			"valueType=\"userObjs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"osp\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.c.compiler.option.preprocessor.def.symbols.1339836817\" "
			"superClass=\"gnu.c.compiler.option.preprocessor.def.symbols\" valueType=\"definedSymbols\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"NDEBUG\"/>\n";
	ss << releaseDefines;
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<inputType id=\"sbi.gnu.c.compiler.tizen.inputType.820536754\" "
			"superClass=\"sbi.gnu.c.compiler.tizen.inputType\"/>\n";
	ss << "\t\t\t\t\t\t\t</tool>\n";
	ss << "\t\t\t\t\t\t\t<tool id=\"org.tizen.nativeide.tool.sbi.gnu.c.linker.base.2099034624\" "
			"name=\"C Linker\" superClass=\"org.tizen.nativeide.tool.sbi.gnu.c.linker.base\"/>\n";
	ss << "\t\t\t\t\t\t\t<tool command=\"clang++\" id=\"org.tizen.nativecpp.tool.sbi.gnu.cpp.linker.933862880\" "
			"name=\"C++ Linker\" superClass=\"org.tizen.nativecpp.tool.sbi.gnu.cpp.linker\">\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.linker.option.frameworks_lflags.cpp.1669519992\" "
			"name=\"Tizen-Frameworks-Other-Lflags\" superClass=\"sbi.gnu.cpp.linker.option.frameworks_lflags.cpp\" "
			"valueType=\"stringList\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-target arm-tizen-linux-gnueabi "
			"-gcc-toolchain &quot;${SBI_SYSROOT}/../../../../tools/arm-linux-gnueabi-gcc-4.5/&quot; "
			"-ccc-gcc-name arm-linux-gnueabi-g++ -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -mtune=cortex-a8 -Xlinker --as-needed\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-pie -lpthread \"/>\n";
//	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-Xlinker -rpath=&quot;/opt/usr/apps/ClJggHLRyf/lib&quot;\"/>\n";
//	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-Xlinker -rpath=&quot;/home/developer/sdk_tools/lib&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"--sysroot=&quot;${SBI_SYSROOT}&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-L&quot;${SBI_SYSROOT}/usr/lib&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-L&quot;${SBI_SYSROOT}/usr/lib/osp&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-uifw\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-appfw\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-image\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-json\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-ime\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-net\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-lpthread\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-content\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-locations\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-telephony\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-uix\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-media\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-messaging\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-web\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-social\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-wifi\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-bluetooth\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-nfc\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-face\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-secure-element\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-speech-tts\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-shell\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-shell-core\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-speech-stt\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-vision\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-lxml2\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-lalut\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-lopenal\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.link.option.paths.71180484\" "
			"name=\"Library search path (-L)\" superClass=\"gnu.cpp.link.option.paths\" valueType=\"libPaths\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"" << xmlEscape(pathConcat(prjDir, "lib"))
			<< "\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.link.option.libs.604111176\" "
			"superClass=\"gnu.cpp.link.option.libs\" valueType=\"libs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"rt\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<inputType id=\"cdt.managedbuild.tool.gnu.cpp.linker.input.936282359\" "
			"superClass=\"cdt.managedbuild.tool.gnu.cpp.linker.input\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<additionalInput kind=\"additionalinputdependency\" paths=\"$(USER_OBJS)\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<additionalInput kind=\"additionalinput\" paths=\"$(LIBS)\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</inputType>\n";
	ss << "\t\t\t\t\t\t\t</tool>\n";
	ss << "\t\t\t\t\t\t\t<tool command=\"arm-linux-gnueabi-as\" "
			"id=\"org.tizen.nativeapp.tool.sbi.gnu.assembler.base.1853895615\" name=\"Assembler\" "
			"superClass=\"org.tizen.nativeapp.tool.sbi.gnu.assembler.base\">\n";
	ss << "\t\t\t\t\t\t\t\t<inputType id=\"cdt.managedbuild.tool.gnu.assembler.input.670124404\" "
			"superClass=\"cdt.managedbuild.tool.gnu.assembler.input\"/>\n";
	ss << "\t\t\t\t\t\t\t</tool>\n";
	ss << "\t\t\t\t\t\t\t<tool id=\"org.tizen.nativecpp.tool.sbi.po.compiler.1152556271\" "
			"name=\"PO Resource Compiler\" superClass=\"org.tizen.nativecpp.tool.sbi.po.compiler\"/>\n";
	ss << "\t\t\t\t\t\t\t<tool id=\"org.tizen.nativecpp.tool.sbi.edc.compiler.821274748\" "
			"name=\"EDC Resource Compiler\" superClass=\"org.tizen.nativecpp.tool.sbi.edc.compiler\"/>\n";
	ss << "\t\t\t\t\t\t</toolChain>\n";
	ss << "\t\t\t\t\t</folderInfo>\n";
	ss << "\t\t\t\t\t<sourceEntries>\n";
	ss << "\t\t\t\t\t\t<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"src\"/>\n";
	ss << "\t\t\t\t\t\t<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"inc\"/>\n";
	ss << "\t\t\t\t\t\t<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"res\"/>\n";
	ss << "\t\t\t\t\t</sourceEntries>\n";
	ss << "\t\t\t\t</configuration>\n";
	ss << "\t\t\t</storageModule>\n";
	ss << "\t\t\t<storageModule moduleId=\"org.eclipse.cdt.core.externalSettings\"/>\n";
	ss << "\t\t</cconfiguration>\n";
	ss << "\t\t<cconfiguration id=\"org.tizen.nativecpp.config.sbi.gcc45.app.debug.da.1697480255\">\n";
	ss << "\t\t\t<storageModule buildSystemId=\"org.eclipse.cdt.managedbuilder.core.configurationDataProvider\" "
			"id=\"org.tizen.nativecpp.config.sbi.gcc45.app.debug.da.1697480255\" "
			"moduleId=\"org.eclipse.cdt.core.settings\" name=\"Dynamic Analyzer\">\n";
	ss << "\t\t\t\t<externalSettings/>\n";
	ss << "\t\t\t\t<extensions>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.ELF\" point=\"org.eclipse.cdt.core.BinaryParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.GmakeErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.CWDLocator\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.MakeErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.GCCErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.GASErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t\t<extension id=\"org.eclipse.cdt.core.GLDErrorParser\" "
			"point=\"org.eclipse.cdt.core.ErrorParser\"/>\n";
	ss << "\t\t\t\t</extensions>\n";
	ss << "\t\t\t</storageModule>\n";
	ss << "\t\t\t<storageModule moduleId=\"cdtBuildSystem\" version=\"4.0.0\">\n";
	ss << "\t\t\t\t<configuration artifactExtension=\"exe\" artifactName=\"${ProjName}\" "
			"buildArtefactType=\"org.tizen.nativecpp.buildArtefactType.app\" "
			"buildProperties=\"org.eclipse.cdt.build.core.buildType=org.eclipse.cdt.build.core.buildType.debug,"
				"org.eclipse.cdt.build.core.buildArtefactType=org.tizen.nativecpp.buildArtefactType.app\" "
			"description=\"\" id=\"org.tizen.nativecpp.config.sbi.gcc45.app.debug.da.1697480255\" "
			"name=\"Dynamic Analyzer\" parent=\"org.tizen.nativecpp.config.sbi.gcc45.app.debug.da\">\n";
	ss << "\t\t\t\t\t<folderInfo id=\"org.tizen.nativecpp.config.sbi.gcc45.app.debug.da.1697480255.\" "
			"name=\"/\" resourcePath=\"\">\n";
	ss << "\t\t\t\t\t\t<toolChain id=\"org.tizen.nativecpp.toolchain.sbi.gcc45.app.debug.da.2010823157\" "
			"name=\"Tizen Native Toolchain\" superClass=\"org.tizen.nativecpp.toolchain.sbi.gcc45.app.debug.da\">\n";
	ss << "\t\t\t\t\t\t\t<targetPlatform binaryParser=\"org.eclipse.cdt.core.ELF\" "
			"id=\"org.tizen.nativeide.target.sbi.gnu.platform.base.163836325\" osList=\"linux,win32\" "
			"superClass=\"org.tizen.nativeide.target.sbi.gnu.platform.base\"/>\n";
	ss << "\t\t\t\t\t\t\t<builder buildPath=\"" << xmlEscape(pathConcat(prjDir, "Dynamic Analyzer")) << "\" "
			"id=\"org.tizen.nativecpp.target.sbi.gnu.builder.53156520\" keepEnvironmentInBuildfile=\"false\" "
			"managedBuildOn=\"true\" name=\"Tizen Application Builder\" "
			"superClass=\"org.tizen.nativecpp.target.sbi.gnu.builder\"/>\n";
	ss << "\t\t\t\t\t\t\t<tool id=\"org.tizen.nativecpp.tool.sbi.gnu.archiver.224081432\" name=\"Archiver\" "
			"superClass=\"org.tizen.nativecpp.tool.sbi.gnu.archiver\"/>\n";
	ss << "\t\t\t\t\t\t\t<tool command=\"clang++\" "
			"id=\"org.tizen.nativecpp.tool.sbi.gnu.cpp.compiler.da.157019183\" name=\"C++ Compiler\" "
			"superClass=\"org.tizen.nativecpp.tool.sbi.gnu.cpp.compiler.da\">\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.compiler.option.optimization.level.1566347494\" "
			"name=\"Optimization Level\" superClass=\"gnu.cpp.compiler.option.optimization.level\" "
			"value=\"gnu.cpp.compiler.optimization.level.none\" valueType=\"enumerated\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.debugging.level.1039215369\" "
			"name=\"Debug level\" superClass=\"sbi.gnu.cpp.compiler.option.debugging.level\" "
			"value=\"gnu.cpp.compiler.debugging.level.max\" valueType=\"enumerated\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.debug.applog.20826248\" "
			"name=\"Enable application logging (-D_APP_LOG)\" "
			"superClass=\"sbi.gnu.cpp.compiler.option.debug.applog\" value=\"true\" valueType=\"boolean\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.2094579273\" "
			"name=\"Tizen-Target\" superClass=\"sbi.gnu.cpp.compiler.option\" valueType=\"userObjs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"tizen-device-2.2.native_llvm31.armel.cpp.app\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.frameworks_inc.cpp.575612279\" "
			"name=\"Tizen-Frameworks-Include-Path\" superClass=\"sbi.gnu.cpp.compiler.option.frameworks_inc.cpp\" "
			"valueType=\"includePath\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include/libxml2&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include/osp&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.frameworks_cflags.cpp.1798520740\" "
			"name=\"Tizen-Frameworks-Other-Cflags\" "
			"superClass=\"sbi.gnu.cpp.compiler.option.frameworks_cflags.cpp\" valueType=\"stringList\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-target arm-tizen-linux-gnueabi "
			"-gcc-toolchain &quot;${SBI_SYSROOT}/../../../../tools/i386-linux-gnueabi-gcc-4.5/&quot; "
			"-ccc-gcc-name arm-linux-gnueabi-g++ -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 "
			"-mtune=cortex-a8 -Wno-gnu\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\" -fPIE\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"--sysroot=&quot;${SBI_SYSROOT}&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.compiler.option.include.paths.1047548940\" "
			"name=\"Include paths (-I)\" superClass=\"gnu.cpp.compiler.option.include.paths\" "
			"valueType=\"includePath\">\n";
	ss << headerPaths;
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;${SDK_INSTALL_PATH}/library&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.compiler.option.preprocessor.def.526257035\" "
			"name=\"Defined symbols (-D)\" superClass=\"gnu.cpp.compiler.option.preprocessor.def\" "
			"valueType=\"definedSymbols\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"_USE_DA_\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"_DEBUG\"/>\n";
	ss << debugDefines;
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.compiler.option.frameworks.cpp.1089674778\" "
			"name=\"Tizen-Frameworks\" superClass=\"sbi.gnu.cpp.compiler.option.frameworks.cpp\" "
			"valueType=\"userObjs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"osp\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.compiler.option.optimization.flags.1473761659\" "
			"name=\"Other optimization flags\" superClass=\"gnu.cpp.compiler.option.optimization.flags\" "
			"value=\"-std=c++11\" valueType=\"string\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<inputType id=\"sbi.gnu.cpp.compiler.tizen.inputType.35892278\" "
			"superClass=\"sbi.gnu.cpp.compiler.tizen.inputType\"/>\n";
	ss << "\t\t\t\t\t\t\t</tool>\n";
	ss << "\t\t\t\t\t\t\t<tool command=\"clang\" id=\"org.tizen.nativecpp.tool.sbi.gnu.c.compiler.1810164913\" "
			"name=\"C Compiler\" superClass=\"org.tizen.nativecpp.tool.sbi.gnu.c.compiler\">\n";
	ss << "\t\t\t\t\t\t\t\t<option defaultValue=\"gnu.c.optimization.level.none\" "
			"id=\"gnu.c.compiler.option.optimization.level.949334544\" name=\"Optimization Level\" "
			"superClass=\"gnu.c.compiler.option.optimization.level\" valueType=\"enumerated\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.debugging.level.318672183\" "
			"name=\"Debug level\" superClass=\"sbi.gnu.c.compiler.option.debugging.level\" "
			"value=\"gnu.c.debugging.level.max\" valueType=\"enumerated\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.debug.applog.1027981900\" "
			"name=\"Enable application logging (-D_APP_LOG)\" "
			"superClass=\"sbi.gnu.c.compiler.option.debug.applog\" value=\"true\" valueType=\"boolean\"/>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.979071863\" "
			"name=\"Tizen-Target\" superClass=\"sbi.gnu.c.compiler.option\" valueType=\"userObjs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"tizen-device-2.2.native_llvm31.armel.cpp.app\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.frameworks_inc.cpp.1259309763\" "
			"name=\"Tizen-Frameworks-Include-Path\" superClass=\"sbi.gnu.c.compiler.option.frameworks_inc.cpp\" "
			"valueType=\"includePath\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include/libxml2&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SBI_SYSROOT}/usr/include/osp&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.frameworks_cflags.cpp.1011957824\" "
			"name=\"Tizen-Frameworks-Other-Cflags\" superClass=\"sbi.gnu.c.compiler.option.frameworks_cflags.cpp\" "
			"valueType=\"stringList\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-target arm-tizen-linux-gnueabi "
			"-gcc-toolchain &quot;/${SBI_SYSROOT}/tools/arm-linux-gnueabi-gcc-4.5/&quot; "
			"-ccc-gcc-name arm-linux-gnueabi-g++ -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 "
			"-mtune=cortex-a8 -Xlinker --as-needed -Wno-gnu\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\" -fPIE\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"--sysroot=&quot;${SBI_SYSROOT}&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.c.compiler.option.include.paths.1170150059\" "
			"name=\"Include paths (-I)\" superClass=\"gnu.c.compiler.option.include.paths\" "
			"valueType=\"includePath\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;${SDK_INSTALL_PATH}/library&quot;\"/>\n";
	ss << headerPaths;
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.c.compiler.option.preprocessor.def.symbols.1936077487\" "
			"name=\"Defined symbols (-D)\" superClass=\"gnu.c.compiler.option.preprocessor.def.symbols\" "
			"valueType=\"definedSymbols\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"_USE_DA_\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"_DEBUG\"/>\n";
	ss << debugDefines;
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.c.compiler.option.frameworks.cpp.554496147\" "
			"name=\"Tizen-Frameworks\" superClass=\"sbi.gnu.c.compiler.option.frameworks.cpp\" "
			"valueType=\"userObjs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"osp\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<inputType id=\"sbi.gnu.c.compiler.tizen.inputType.2063186423\" "
			"superClass=\"sbi.gnu.c.compiler.tizen.inputType\"/>\n";
	ss << "\t\t\t\t\t\t\t</tool>\n";
	ss << "\t\t\t\t\t\t\t<tool id=\"org.tizen.nativeide.tool.sbi.gnu.c.linker.base.1619389375\" "
			"name=\"C Linker\" superClass=\"org.tizen.nativeide.tool.sbi.gnu.c.linker.base\"/>\n";
	ss << "\t\t\t\t\t\t\t<tool command=\"clang++\" "
			"id=\"org.tizen.nativecpp.tool.sbi.gnu.cpp.linker.da.2127786116\" name=\"C++ Linker\" "
			"superClass=\"org.tizen.nativecpp.tool.sbi.gnu.cpp.linker.da\">\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"sbi.gnu.cpp.linker.option.frameworks_lflags.cpp.153910298\" "
			"name=\"Tizen-Frameworks-Other-Lflags\" superClass=\"sbi.gnu.cpp.linker.option.frameworks_lflags.cpp\" "
			"valueType=\"stringList\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-target arm-tizen-linux-gnueabi "
			"-gcc-toolchain &quot;${SBI_SYSROOT}/tools/arm-linux-gnueabi-gcc-4.5/&quot; "
			"-ccc-gcc-name arm-linux-gnueabi-g++ -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 "
			"-mtune=cortex-a8 -Xlinker --as-needed -Xlinker --as-needed\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-pie -lpthread \"/>\n";
//	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-Xlinker -rpath=&quot;/opt/usr/apps/ClJggHLRyf/lib&quot;\"/>\n";
//	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-Xlinker -rpath=&quot;/home/developer/sdk_tools/lib&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"--sysroot=&quot;${SBI_SYSROOT}&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-L&quot;${SBI_SYSROOT}/usr/lib&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-L&quot;${SBI_SYSROOT}/usr/lib/osp&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-uifw\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-appfw\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-image\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-json\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-ime\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-net\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-lpthread\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-content\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-locations\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-telephony\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-uix\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-media\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-messaging\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-web\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-social\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-wifi\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-bluetooth\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-nfc\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-face\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-secure-element\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-speech-tts\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-shell\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-shell-core\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-speech-stt\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-losp-vision\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-lxml2\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-lalut\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"-lopenal\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.link.option.libs.2140324894\" "
			"name=\"Libraries (-l)\" superClass=\"gnu.cpp.link.option.libs\" valueType=\"libs\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"daprobe\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"rt\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<option id=\"gnu.cpp.link.option.paths.2016772773\" "
			"name=\"Library search path (-L)\" superClass=\"gnu.cpp.link.option.paths\" valueType=\"libPaths\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"" << xmlEscape(pathConcat(prjDir, "lib")) << "\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<listOptionValue builtIn=\"false\" "
			"value=\"&quot;${SDK_INSTALL_PATH}/library/da/arm&quot;\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</option>\n";
	ss << "\t\t\t\t\t\t\t\t<inputType id=\"cdt.managedbuild.tool.gnu.cpp.linker.input.1880261431\" "
			"superClass=\"cdt.managedbuild.tool.gnu.cpp.linker.input\">\n";
	ss << "\t\t\t\t\t\t\t\t\t<additionalInput kind=\"additionalinputdependency\" paths=\"$(USER_OBJS)\"/>\n";
	ss << "\t\t\t\t\t\t\t\t\t<additionalInput kind=\"additionalinput\" paths=\"$(LIBS)\"/>\n";
	ss << "\t\t\t\t\t\t\t\t</inputType>\n";
	ss << "\t\t\t\t\t\t\t</tool>\n";
	ss << "\t\t\t\t\t\t\t<tool command=\"arm-linux-gnueabi-as\" "
			"id=\"org.tizen.nativeapp.tool.sbi.gnu.assembler.base.1675585359\" name=\"Assembler\" "
			"superClass=\"org.tizen.nativeapp.tool.sbi.gnu.assembler.base\">\n";
	ss << "\t\t\t\t\t\t\t\t<inputType id=\"cdt.managedbuild.tool.gnu.assembler.input.656300921\" "
			"superClass=\"cdt.managedbuild.tool.gnu.assembler.input\"/>\n";
	ss << "\t\t\t\t\t\t\t</tool>\n";
	ss << "\t\t\t\t\t\t\t<tool id=\"org.tizen.nativecpp.tool.sbi.po.compiler.1238695036\" "
			"name=\"PO Resource Compiler\" superClass=\"org.tizen.nativecpp.tool.sbi.po.compiler\"/>\n";
	ss << "\t\t\t\t\t\t\t<tool id=\"org.tizen.nativecpp.tool.sbi.edc.compiler.1357517574\" "
			"name=\"EDC Resource Compiler\" superClass=\"org.tizen.nativecpp.tool.sbi.edc.compiler\"/>\n";
	ss << "\t\t\t\t\t\t</toolChain>\n";
	ss << "\t\t\t\t\t</folderInfo>\n";
	ss << "\t\t\t\t\t<sourceEntries>\n";
	ss << "\t\t\t\t\t\t<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"src\"/>\n";
	ss << "\t\t\t\t\t\t<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"inc\"/>\n";
	ss << "\t\t\t\t\t\t<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"res\"/>\n";
	ss << "\t\t\t\t\t</sourceEntries>\n";
	ss << "\t\t\t\t</configuration>\n";
	ss << "\t\t\t</storageModule>\n";
	ss << "\t\t\t<storageModule moduleId=\"org.eclipse.cdt.core.externalSettings\"/>\n";
	ss << "\t\t</cconfiguration>\n";
	ss << "\t</storageModule>\n";
	ss << "\t<storageModule moduleId=\"cdtBuildSystem\" version=\"4.0.0\">\n";
	ss << "\t\t<project id=\"hackaphone.org.tizen.nativecpp.target.sbi.gcc45.app.1514845230\" "
			"name=\"Tizen Application\" projectType=\"org.tizen.nativecpp.target.sbi.gcc45.app\"/>\n";
	ss << "\t</storageModule>\n";
	ss << "\t<storageModule moduleId=\"scannerConfiguration\">\n";
	ss << "\t\t<autodiscovery enabled=\"true\" problemReportingEnabled=\"true\" selectedProfileId=\"\"/>\n";
	ss << "\t\t<scannerConfigBuildInfo "
			"instanceId=\"org.tizen.nativecpp.config.sbi.gcc45.app.debug.da.1697480255\">\n";
	ss << "\t\t\t<autodiscovery enabled=\"true\" problemReportingEnabled=\"true\" "
			"selectedProfileId=\"org.tizen.nativecommon.TizenGCCManagedMakePerProjectProfileCPP\"/>\n";
	ss << "\t\t</scannerConfigBuildInfo>\n";
	ss << "\t\t<scannerConfigBuildInfo instanceId=\"org.tizen.nativecpp.config.sbi.gcc45.app.release.598345466\">\n";
	ss << "\t\t\t<autodiscovery enabled=\"true\" problemReportingEnabled=\"true\" "
			"selectedProfileId=\"org.tizen.nativecommon.TizenGCCManagedMakePerProjectProfileCPP\"/>\n";
	ss << "\t\t</scannerConfigBuildInfo>\n";
	ss << "\t\t<scannerConfigBuildInfo instanceId=\"org.tizen.nativecpp.config.sbi.gcc45.app.debug.1450051639\">\n";
	ss << "\t\t\t<autodiscovery enabled=\"true\" problemReportingEnabled=\"true\" "
			"selectedProfileId=\"org.tizen.nativecommon.TizenGCCManagedMakePerProjectProfileCPP\"/>\n";
	ss << "\t\t</scannerConfigBuildInfo>\n";
	ss << "\t</storageModule>\n";
	ss << "\t<storageModule moduleId=\"com.samsung.tizen.nativeapp.projectInfo\" version=\"1.0.0\"/>\n";
	ss << "\t<storageModule moduleId=\"refreshScope\"/>\n";
	ss << "</cproject>\n";
	project->yipDirectory()->writeFile(pathConcat(projectName, ".cproject"), ss.str());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generating

void Gen::generate()
{
	projectName = "tizen";
	projectPath = pathConcat(project->yipDirectory()->path(), projectName);

	generateSrcFiles();
	deleteOldSrcFiles("src", pathConcat(projectPath, "src"));

	writeManifest();

	writeProjectFile();
	writeCProjectFile();
}

std::string generateTizen(const ProjectPtr & project)
{
	Gen gen;
	gen.project = project;
	gen.generate();
	return gen.projectPath;
}
