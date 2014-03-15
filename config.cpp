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
#include "config.h"
#include "util/path.h"
#include "util/fmt.h"
#include "3rdparty/inih/ini.h"
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <sstream>
#include <iostream>

static const char * CONFIG_DIRECTORY = ".yip";
static const char * CONFIG_FILE_NAME = "yip.conf";

static const char * SECTION_GLOBAL = "global";
static const char * OPTION_PROJECT_FILE_NAME = "project_file_name";

static const char * SECTION_REPOSITORIES = "repo";

static const char * PROJECT_FILE_NAME = "Yipfile";

ConfigPtr g_Config;

/* Config */

Config::Config()
	: projectFileName(PROJECT_FILE_NAME)
{
	repos.insert(std::make_pair("box2d", "https://github.com/oss-forks/box2d.git"));
	repos.insert(std::make_pair("sqlite3", "https://github.com/oss-forks/sqlite3.git"));
	repos.insert(std::make_pair("stb_image", "https://github.com/oss-forks/stb_image.git"));
	repos.insert(std::make_pair("tinyxml", "https://github.com/oss-forks/tinyxml.git"));
	repos.insert(std::make_pair("zlib", "https://github.com/oss-forks/zlib.git"));

	repos.insert(std::make_pair("cxx-util", "https://github.com/zapolnov/yip-cxx-util.git"));
	repos.insert(std::make_pair("game-main", "https://github.com/zapolnov/yip-game-main.git"));
	repos.insert(std::make_pair("gl_wrappers", "https://github.com/zapolnov/yip-gl-wrappers.git"));
	repos.insert(std::make_pair("math", "https://github.com/zapolnov/yip-math.git"));
	repos.insert(std::make_pair("stb_image_cxx", "https://github.com/zapolnov/yip-stb-image-cxx.git"));
}

struct Context
{
	Config * config;
	std::string errorMsg;
	std::exception_ptr exception;
	bool hasException;
};

enum
{
	Ok = 1,
	Error = 0
};

static int iniHandler(void * user, const char * section, const char * name, const char * value)
{
	Context * context = reinterpret_cast<Context *>(user);
	try
	{
		if (!strcmp(section, SECTION_GLOBAL))
		{
			if (!strcmp(name, OPTION_PROJECT_FILE_NAME))
			{
				context->config->projectFileName = value;
				return Ok;
			}
		}
		else if (!strcmp(section, SECTION_REPOSITORIES))
		{
			auto r = context->config->repos.insert(std::make_pair(name, value));
			if (!r.second)
				r.first->second = value;
			return Ok;
		}

		context->errorMsg = fmt() << "unknown parameter '" << section << '/' << name << "'.";
		return Error;
	}
	catch (...)
	{
		context->exception = std::current_exception();
		context->hasException = true;
		return Error;
	}
}

bool Config::read(const std::string & path)
{
	FILE * f = fopen(path.c_str(), "r");
	if (!f)
		return false;

	try
	{
		Context context;
		context.hasException = false;
		context.errorMsg = "syntax error.";
		context.config = this;

		int errline = ini_parse_file(f, iniHandler, &context);
		if (errline != 0)
		{
			if (context.hasException)
				std::rethrow_exception(context.exception);

			if (errline < 0)
			{
				throw std::runtime_error(fmt()
					<< "unable to parse file '" << pathToNativeSeparators(path) << "': internal error.");
			}

			throw std::runtime_error(fmt() << "in file '"
				<< pathToNativeSeparators(path) << "' at line " << errline << ": " << context.errorMsg);
		}
	}
	catch (...)
	{
		fclose(f);
		throw;
	}

	fclose(f);

	return true;
}

void Config::write(const std::string & path)
{
	std::stringstream ss;

	ss << '\n';
	ss << "[" << SECTION_GLOBAL << "]\n";
	ss << OPTION_PROJECT_FILE_NAME << " = " << projectFileName << '\n';
	ss << '\n';

	ss << "[" << SECTION_REPOSITORIES << "]\n";
	for (auto it = repos.begin(); it != repos.end(); ++it)
		ss << it->first << " = " << it->second << "\n";

	std::string text = ss.str();

	FILE * f = fopen(path.c_str(), "w");
	if (!f)
	{
		int err = errno;
		throw std::runtime_error(fmt()
			<< "unable to create file '" << pathToNativeSeparators(path) << "': " << strerror(err));
	}

	try
	{
		fwrite(text.data(), 1, text.length(), f);
		if (ferror(f))
		{
			int err = errno;
			throw std::runtime_error(fmt()
				<< "unable to write file '" << pathToNativeSeparators(path) << "': " << strerror(err));
		}
	}
	catch (...)
	{
		fclose(f);
		remove(path.c_str());
		throw;
	}

	fclose(f);

	std::cout << "wrote default configuration to '" << pathToNativeSeparators(path) << "'." << std::endl;
}


/* Functions */

ConfigPtr loadConfigFile(bool create)
{
	ConfigPtr config = std::make_shared<Config>();

	std::string configPath = pathConcat(pathGetUserHomeDirectory(), CONFIG_DIRECTORY);
	std::string configFile = pathConcat(configPath, CONFIG_FILE_NAME);
	if (create && pathCreate(configPath))
	{
		std::cout << "created directory '" << pathToNativeSeparators(configPath) << "'." << std::endl;
		config->write(configFile);
	}
	else if (!config->read(configFile))
	{
		int err = errno;
		std::cerr << "warning: unable to open file '"
			<< pathToNativeSeparators(configFile) << "': " << strerror(err);
	}

	return config;
}

void loadConfig()
{
	g_Config = loadConfigFile(true);
}
