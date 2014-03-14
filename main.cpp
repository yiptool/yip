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
#include "project/project_file_parser.h"
#include "3rdparty/libgit2/include/git2/threads.h"
#include "util/fmt.h"
#include "config.h"
#include <exception>
#include <fstream>
#include <iostream>

static ProjectFilePtr loadProjectFile()
{
	ProjectFilePtr projectFile = std::make_shared<ProjectFile>();

	ProjectFileParser parser(g_Config->projectFileName);
	parser.parse(projectFile);

	return projectFile;
}

static void usage()
{
	std::cout <<
		"Usage: yip [command] [options]\n"
		"\n"
		"The following commands are available:\n"
		"\n"
		"   build (default)       Build the project.\n"
		"   help                  Display this help message.\n"
		"   update                Download latest versions of dependencies.\n"
		<< std::endl;
}

static int build(int, char **)
{
	ProjectFilePtr projectFile = loadProjectFile();
	return 0;
}

static int help(int, char **)
{
	usage();
	return 1;
}

static int update(int, char **)
{
	ProjectFilePtr projectFile = loadProjectFile();

	if (projectFile->repositories().size() == 0)
	{
		std::cout << "nothing to update.";
		return 0;
	}

	GitProgressPrinter printer;
	for (const GitRepositoryPtr & repo : projectFile->repositories())
		repo->fetch("origin", &printer);

	return 0;
}

int main(int argc, char ** argv)
{
	try
	{
		git_threads_init();
		loadConfig();

		for (int i = 1; i < argc; i++)
		{
			if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") || !strcmp(argv[i], "/?"))
			{
				usage();
				return 1;
			}
		}

		if (argc > 1 && argv[1][0] != '-')
		{
			std::unordered_map<std::string, int (*)(int, char **)> commands;
			commands.insert(std::make_pair("build", &build));
			commands.insert(std::make_pair("help", &help));
			commands.insert(std::make_pair("update", &update));

			auto it = commands.find(argv[1]);
			if (it == commands.end())
				throw std::runtime_error(fmt() << "unknown command '" << argv[1] << "'. try 'yip help'.");

			return it->second(argc - 2, argv + 2);
		}

		build(argc - 1, argv + 1);
	}
	catch (const std::exception & e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
