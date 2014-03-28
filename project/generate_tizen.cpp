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
#include "../util/sha1.h"
#include "../util/path.h"
#include <sstream>
#include <unordered_set>

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
		void deleteOldSrcFiles(const std::string & path);

		void writeMakefileInit();
		void writeManifest();

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Gen::generateSrcFiles()
{
	std::string srcDir = pathConcat(projectName, "src");
	for (auto it : project->sourceFiles())
	{
		const SourceFilePtr & file = it.second;

		if (!(file->platforms() & Platform::Tizen))
			continue;
		if (!isCompilableFileType(file->type()))
			continue;

		std::string name = pathConcat(srcDir, file->name());

		std::string ext = extensionForFileType(file->type());
		if (pathGetFullFileExtension(file->name()) != ext)
			name = fmt() << file->name() << '.' << sha1(file->name()).substr(0, 4) << ext;

		std::string path = pathSimplify(pathConcat(project->yipDirectory()->path(), name));

		pathCreate(pathGetDirectory(path));
		pathCreateSymLink(file->path(), path);

		srcFiles.insert(pathToUnixSeparators(path));
	}
}

void Gen::deleteOldSrcFiles(const std::string & path)
{
	DirEntryList list = pathEnumDirectoryContents(path);
	for (auto it : list)
	{
		std::string file = pathConcat(path, it.name);
		if (it.type == DirEntry_Directory)
			deleteOldSrcFiles(file);

		if (srcFiles.find(file) == srcFiles.end())
		{
			printf("<<%s>>\n", file.c_str());
//			unlink(file.c_str());
		}
	}
}

static std::string escapeQuote(const std::string & str)
{
	std::stringstream ss;
	for (char ch : str)
	{
		if (ch != '"')
			ss << ch;
		else
			ss << "\\\"";
	}
	return ss.str();
}

void Gen::writeMakefileInit()
{
	std::string incPath1 = project->yipDirectory()->path();
	std::string incPath2 = pathConcat(incPath1, ".yip-import-proxies");

	std::stringstream ss;
	ss << "CC := $(CC) -std=c++11 -I\"" << incPath1 << "\" -I\"" << incPath2 << "\"";
	for (auto it : project->headerPaths())
	{
		if ((!it.second->platforms() & Platform::Tizen))
			continue;
		ss << " -I\"" << escapeQuote(it.second->path()) << '"';
	}
	ss << " -D__TIZEN__";
	for (auto it : project->defines())
	{
		const DefinePtr & define = it.second;
		if (!(define->platforms() & Platform::Tizen))
			continue;

//		if (define->buildTypes() & BuildType::Debug)		// FIXME
//		if (define->buildTypes() & BuildType::Release)
		ss << " \"-D" << escapeQuote(define->name()) << '"';
	}
	ss << '\n';
	ss << "TC_LINKER_MISC += -lrt\n";
	project->yipDirectory()->writeFile(pathConcat(projectName, "makefile.init"), ss.str());
}

void Gen::writeManifest()
{
	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";
	ss << "<Manifest xmlns=\"http://schemas.tizen.org/2012/12/manifest\">\n";
	ss << "\t<Id>$(manifestAppId)</Id>\n";
	ss << "\t<Version>2.2.0</Version>\n";
	ss << "\t<Type>C++App</Type>\n";
	ss << "\t<Requirements>\n";
	ss << "\t\t<Feature Name=\"http://tizen.org/feature/screen.size.all\">true</Feature>\n";
	ss << "\t</Requirements>\n";
	ss << "\t<Apps>\n";
	ss << "\t\t<ApiVersion>2.2</ApiVersion>\n";
	ss << "\t\t<Privileges />\n";
	ss << "\t\t<UiApp Main=\"True\" MenuIconVisible=\"True\" Name=\"$(projectName)\">\n";
	ss << "\t\t\t<UiScalability CoordinateSystem=\"Physical\" />\n";
	ss << "\t\t\t<UiTheme SystemTheme=\"Black\" />\n";
	ss << "\t\t\t<DisplayNames>\n";
	ss << "\t\t\t\t<DisplayName Locale=\"eng-GB\">$(projectName)</DisplayName>\n";
	ss << "\t\t\t</DisplayNames>\n";
	ss << "\t\t\t<Icons>\n";
	ss << "\t\t\t\t<Icon Section=\"MainMenu\" Type=\"Xhigh\">mainmenu.png</Icon>\n";
	ss << "\t\t\t</Icons>\n";
	ss << "\t\t\t<LaunchConditions />\n";
	ss << "\t\t\t<Notifications />\n";
	ss << "\t\t</UiApp>\n";
	ss << "\t</Apps>\n";
	ss << "</Manifest>\n";
	project->yipDirectory()->writeFile(pathConcat(projectName, "manifest.xml"), ss.str());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generating

void Gen::generate()
{
	projectName = "tizen";
	projectPath = pathConcat(project->yipDirectory()->path(), projectName);

	generateSrcFiles();
	deleteOldSrcFiles(pathConcat(projectPath, "src"));

	writeMakefileInit();
	writeManifest();
}

std::string generateTizen(const ProjectPtr & project)
{
	Gen gen;
	gen.project = project;
	gen.generate();
	return gen.projectPath;
}
