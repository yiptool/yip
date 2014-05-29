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
#include "resource_compiler.h"
#include "../util/path.h"
#include "../util/cxx_escape.h"
#include "../util/fmt.h"
#include "../util/sha1.h"
#include <cstdio>
#include <stdexcept>
#include <sstream>
#include <unordered_map>
#include <iomanip>

#define SKIP_PLATFORMS (Platform::iOS | Platform::OSX | Platform::Android)

typedef std::unordered_map<std::string, std::pair<std::string, Platform::Type>> ResCatalog;

static void makeResourceFile(ResCatalog & cat, const ProjectPtr & project, const SourceFilePtr & resourceFile)
{
	std::string yipDir = project->yipDirectory()->path();

	std::string targetName = sha1(resourceFile->name());
	std::string targetPath = pathConcat(".yip-resources", targetName) + ".cpp";

	// Add file into the catalog

	cat.insert(std::make_pair(resourceFile->name(), std::make_pair(targetName, resourceFile->platforms())));

	// Do not regenerate output file if input file did not change

	if (!project->yipDirectory()->shouldProcessFile(targetPath, resourceFile->path(), false))
	{
		SourceFilePtr sourceFile = project->addSourceFile(targetPath, pathConcat(yipDir, targetPath));
		sourceFile->setIsGenerated(true);
		sourceFile->setPlatforms(resourceFile->platforms() & ~SKIP_PLATFORMS);
		return;
	}

	// Load the input file

	FILE * f = fopen(resourceFile->path().c_str(), "rb");
	if (!f)
		throw std::runtime_error(fmt() << "unable to open file '" << resourceFile->path() << "'.");

	std::vector<unsigned char> buf;
	try
	{
		fseek(f, 0, SEEK_END);
		long size = ftell(f);
		fseek(f, 0, SEEK_SET);

		if (ferror(f) || size < 0)
			throw std::runtime_error(fmt() << "unable to determine size of file '" << resourceFile->path() << "'.");

		buf.resize(static_cast<size_t>(size));
		size_t bytesRead = fread(buf.data(), 1, static_cast<size_t>(size), f);
		if (ferror(f))
			throw std::runtime_error(fmt() << "unable to read file '" << resourceFile->path() << "'.");
		buf.resize(bytesRead);
	}
	catch (...)
	{
		fclose(f);
		throw;
	}

	fclose(f);

	// Generate the output file

	std::stringstream ss;
	ss << "#include <cstddef>\n";
	ss << "extern const size_t __yip_resource_size_" << targetName << " = " << buf.size() << ";\n";
	ss << "extern const unsigned char __yip_resource_" << targetName << "[] = {";
	size_t offset = 0;
	for (unsigned char ch : buf)
	{
		if (offset++ % 32 == 0)
			ss << '\n';
		ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ch) << ',';
	}
	ss << "};\n";

	// Write the output file

	std::string generatedPath = project->yipDirectory()->writeFile(targetPath, ss.str());
	SourceFilePtr sourceFile = project->addSourceFile(targetPath, generatedPath);
	sourceFile->setIsGenerated(true);
	sourceFile->setPlatforms(resourceFile->platforms() & ~SKIP_PLATFORMS);
}

void writeResourceCatalog(const ProjectPtr & project, const ResCatalog & cat, Platform::Type platform)
{
	std::stringstream ss;

	ss << "#include <unordered_map>\n";
	ss << "#include <string>\n";

	for (auto it : cat)
	{
		if (it.second.second & platform)
		{
			ss << '\n';
			ss << "extern const unsigned char __yip_resource_" << it.second.first << "[];\n";
			ss << "extern const size_t __yip_resource_size_" << it.second.first << ";\n";
		}
	}

	ss << '\n';
	ss << "std::unordered_map<std::string, std::pair<const void *, size_t>> __yip_resources = {\n";

	for (auto it : cat)
	{
		if (!(it.second.second & platform))
			continue;
		ss << "\t{ \"";
		cxxEscape(ss, it.first);
		ss << "\", { __yip_resource_" << it.second.first << ", __yip_resource_size_" << it.second.first << " } },\n";
	}

	ss << "};\n";

	std::string name = fmt() << ".yip-resources/catalog_"
		<< std::hex << std::setw(4) << std::setfill('0') << platform << ".cpp";

	std::string generatedPath = project->yipDirectory()->writeFile(name, ss.str());
	SourceFilePtr sourceFile = project->addSourceFile(name, generatedPath);
	sourceFile->setIsGenerated(true);
	sourceFile->setPlatforms(platform);
}

void compileResources(const ProjectPtr & project)
{
	ResCatalog cat;

	for (auto it : project->resourceFiles())
	{
		const SourceFilePtr & file = it.second;
		if ((file->platforms() & ~SKIP_PLATFORMS) == 0)
			continue;
		makeResourceFile(cat, project, file);
	}

	for (size_t i = 1; i < 0xFFFF; i <<= 1)
	{
		if ((i & SKIP_PLATFORMS) != 0)
			continue;
		writeResourceCatalog(project, cat, static_cast<Platform::Type>(i));
	}
}
