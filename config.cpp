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
#include "util/path-util/path-util.h"
#include "util/cxx-util/cxx-util/fmt.h"
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
	repos.insert(std::make_pair("dirent", "https://github.com/oss-forks/dirent.git"));
	repos.insert(std::make_pair("glm", "https://github.com/oss-forks/glm.git"));
	repos.insert(std::make_pair("font-opensans-bold", "https://github.com/oss-forks/font-opensans-bold.git"));
	repos.insert(std::make_pair("font-opensans-light", "https://github.com/oss-forks/font-opensans-light.git"));
	repos.insert(std::make_pair("font-opensans-regular", "https://github.com/oss-forks/font-opensans-regular.git"));
	repos.insert(std::make_pair("font-opensans-semibold", "https://github.com/oss-forks/font-opensans-semibold.git"));
	repos.insert(std::make_pair("font-opensans-semibolditalic", "https://github.com/oss-forks/font-opensans-semibolditalic.git"));
	repos.insert(std::make_pair("imgui", "https://github.com/oss-forks/imgui.git"));
	repos.insert(std::make_pair("inih", "https://github.com/oss-forks/inih.git"));
	repos.insert(std::make_pair("ios-form-sheet-controller", "https://github.com/oss-forks/ios-form-sheet-controller.git"));
	repos.insert(std::make_pair("ios-keyboard-avoiding", "https://github.com/oss-forks/ios-keyboard-avoiding.git"));
	repos.insert(std::make_pair("ios-refresh-control", "https://github.com/oss-forks/ios-refresh-control.git"));
	repos.insert(std::make_pair("ios-star-rating-view", "https://github.com/oss-forks/ios-star-rating-view.git"));
	repos.insert(std::make_pair("ios-twitter-auth", "https://github.com/oss-forks/ios-twitter-auth.git"));
	repos.insert(std::make_pair("ios-vk-sdk", "https://github.com/oss-forks/ios-vk-sdk.git"));
	repos.insert(std::make_pair("kissfft", "https://github.com/oss-forks/kissfft.git"));
	repos.insert(std::make_pair("libogg", "https://github.com/oss-forks/libogg.git"));
	repos.insert(std::make_pair("libpng", "https://github.com/oss-forks/libpng.git"));
	repos.insert(std::make_pair("lua", "https://github.com/oss-forks/lua.git"));
	repos.insert(std::make_pair("dhpoware-modelobj", "https://github.com/oss-forks/dhpoware-modelobj.git"));
	repos.insert(std::make_pair("micropather", "https://github.com/oss-forks/micropather.git"));
	repos.insert(std::make_pair("mongoose", "https://github.com/oss-forks/mongoose.git"));
	repos.insert(std::make_pair("novocaine", "https://github.com/oss-forks/novocaine.git"));
	repos.insert(std::make_pair("openssl", "https://github.com/oss-forks/openssl.git"));
	repos.insert(std::make_pair("sqlite3", "https://github.com/oss-forks/sqlite3.git"));
	repos.insert(std::make_pair("stb_image", "https://github.com/oss-forks/stb_image.git"));
	repos.insert(std::make_pair("stb_truetype", "https://github.com/oss-forks/stb_truetype.git"));
	repos.insert(std::make_pair("strtod", "https://github.com/oss-forks/strtod.git"));
	repos.insert(std::make_pair("tinyobjloader", "https://github.com/oss-forks/tinyobjloader.git"));
	repos.insert(std::make_pair("tinyxml", "https://github.com/oss-forks/tinyxml.git"));
	repos.insert(std::make_pair("tremor", "https://github.com/oss-forks/tremor.git"));
	repos.insert(std::make_pair("zlib", "https://github.com/oss-forks/zlib.git"));

	repos.insert(std::make_pair("amazon-aws-runtime", "https://github.com/yiptool/amazon-aws-runtime.git"));
	repos.insert(std::make_pair("amazon-aws-s3", "https://github.com/yiptool/amazon-aws-s3.git"));
	repos.insert(std::make_pair("amazon-aws-s3-util", "https://github.com/yiptool/amazon-aws-s3-util.git"));
	repos.insert(std::make_pair("android-jni-util", "https://github.com/yiptool/android-jni-util.git"));
	repos.insert(std::make_pair("android-util", "https://github.com/yiptool/android-util.git"));
	repos.insert(std::make_pair("box2d-debug-renderer", "https://github.com/yiptool/box2d-debug-renderer.git"));
	repos.insert(std::make_pair("cxx-util", "https://github.com/yiptool/cxx-util.git"));
	repos.insert(std::make_pair("facebook-sdk", "https://github.com/yiptool/facebook-sdk.git"));
	repos.insert(std::make_pair("game-main", "https://github.com/yiptool/game-main.git"));
	repos.insert(std::make_pair("gles2", "https://github.com/yiptool/gles2.git"));
	repos.insert(std::make_pair("gles2-util", "https://github.com/yiptool/gles2-util.git"));
	repos.insert(std::make_pair("ios-action-sheet", "https://github.com/yiptool/ios-action-sheet.git"));
	repos.insert(std::make_pair("ios-airplay-util", "https://github.com/yiptool/ios-airplay-util.git"));
	repos.insert(std::make_pair("ios-facebook-util", "https://github.com/yiptool/ios-facebook-util.git"));
	repos.insert(std::make_pair("ios-opengl-view", "https://github.com/yiptool/ios-opengl-view.git"));
	repos.insert(std::make_pair("ios-parse-facebook-util", "https://github.com/yiptool/ios-parse-facebook-util.git"));
	repos.insert(std::make_pair("ios-parse-twitter-util", "https://github.com/yiptool/ios-parse-twitter-util.git"));
	repos.insert(std::make_pair("ios-system-sound", "https://github.com/yiptool/ios-system-sound.git"));
	repos.insert(std::make_pair("ios-twitter-util", "https://github.com/yiptool/ios-twitter-util.git"));
	repos.insert(std::make_pair("ios-util", "https://github.com/yiptool/ios-util.git"));
	repos.insert(std::make_pair("jni-util", "https://github.com/yiptool/jni-util.git"));
	repos.insert(std::make_pair("math", "https://github.com/yiptool/math.git"));
	repos.insert(std::make_pair("ogg-vorbis-stream", "https://github.com/yiptool/ogg-vorbis-stream.git"));
	repos.insert(std::make_pair("parse-com", "https://github.com/yiptool/parse-com.git"));
	repos.insert(std::make_pair("resources", "https://github.com/yiptool/resources.git"));
	repos.insert(std::make_pair("scenegraph", "https://github.com/yiptool/scenegraph.git"));
	repos.insert(std::make_pair("sound", "https://github.com/yiptool/sound.git"));
	repos.insert(std::make_pair("sqlite3-util", "https://github.com/yiptool/sqlite3-util.git"));
	repos.insert(std::make_pair("stb_image_cxx", "https://github.com/yiptool/stb_image_cxx.git"));
	repos.insert(std::make_pair("tinyxml-util", "https://github.com/yiptool/tinyxml-util.git"));

	repos.insert(std::make_pair("audio", "https://github.com/friedcroc/yip-audio.git"));
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
			<< pathToNativeSeparators(configFile) << "': " << strerror(err) << std::endl;
	}

	return config;
}

void loadConfig()
{
	g_Config = loadConfigFile(true);
}
