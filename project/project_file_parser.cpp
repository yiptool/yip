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
#include "project_file_parser.h"
#include "../config.h"
#include "../util/image.h"
#include "../util/cxx-util/cxx-util/fmt.h"
#include "../util/path-util/path-util.h"
#include <unordered_map>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iostream>

enum class ProjectFileParser::Token
{
	Eof = 0,
	LCurly,
	RCurly,
	LParen,
	RParen,
	Colon,
	Exclamation,
	Comma,
	Literal,
	Equal,
	Arrow
};

struct ProjectFileParser::ImageSize
{
	Project::ImageSize size;
	unsigned width;
	unsigned height;
};

struct ProjectFileParser::Error : public std::runtime_error
{
	inline Error(const std::string & message) : std::runtime_error(message) {}
  #if __cplusplus < 201103L || (defined(_MSC_VER) && !defined(__clang__))
	inline ~Error() throw() {}
  #else
	inline ~Error() noexcept {}
  #endif
};

static bool isValidPathPrefix(const std::string & prefix)
{
	if (prefix.length() == 0)
		return false;
	for (char ch : prefix)
	{
		if (ch >= 'a' && ch <= 'z')
			continue;
		if (ch >= 'A' && ch <= 'Z')
			continue;
		if (ch >= '0' && ch <= '9')
			continue;
		if (ch == '.' || ch == '-' || ch == '_')
			continue;
		return false;
	}
	return true;
}

/* ProjectFileParser */

ProjectFileParser::ProjectFileParser(const std::string & filename, const std::string & pathPrefix,
		Platform::Type platform)
	: m_FileName(pathMakeAbsolute(filename)),
	  m_PathPrefix(pathPrefix),
	  m_ProjectPath(pathGetDirectory(m_FileName)),
	  m_DefaultPlatformMask(platform),
	  m_Token(Token::Eof),
	  m_CurLine(1),
	  m_TokenLine(1),
	  m_LastChar(0),
	  m_ResolveImports(false)
{
	m_Stream.open(filename, std::ios::in);
	if (!m_Stream.is_open() || m_Stream.fail() || m_Stream.bad())
		throw std::runtime_error(fmt() << "unable to open file '" << filename << "'.");

	m_CommandHandlers.insert(std::make_pair("project_name", &ProjectFileParser::parseProjectName));
	m_CommandHandlers.insert(std::make_pair("sources", &ProjectFileParser::parseSources));
	m_CommandHandlers.insert(std::make_pair("app_sources", &ProjectFileParser::parseAppSources));
	m_CommandHandlers.insert(std::make_pair("public_headers", &ProjectFileParser::parsePublicHeaders));
	m_CommandHandlers.insert(std::make_pair("defines", &ProjectFileParser::parseDefines));
	m_CommandHandlers.insert(std::make_pair("app_defines", &ProjectFileParser::parseAppDefines));
	m_CommandHandlers.insert(std::make_pair("import", &ProjectFileParser::parseImport));
	m_CommandHandlers.insert(std::make_pair("resources", &ProjectFileParser::parseResources));
	m_CommandHandlers.insert(std::make_pair("resources_dir", &ProjectFileParser::parseResourcesDir));
	m_CommandHandlers.insert(std::make_pair("app_resources", &ProjectFileParser::parseAppResources));
	m_CommandHandlers.insert(std::make_pair("winrt", &ProjectFileParser::parseWinRT));
	m_CommandHandlers.insert(std::make_pair("ios", &ProjectFileParser::parseIOSorOSX));
	m_CommandHandlers.insert(std::make_pair("osx", &ProjectFileParser::parseIOSorOSX));
	m_CommandHandlers.insert(std::make_pair("android", &ProjectFileParser::parseAndroid));
	m_CommandHandlers.insert(std::make_pair("tizen", &ProjectFileParser::parseTizen));
	m_CommandHandlers.insert(std::make_pair("license", &ProjectFileParser::parseLicense));
	m_CommandHandlers.insert(std::make_pair("todo", &ProjectFileParser::parseToDo));
	m_CommandHandlers.insert(std::make_pair("translation_file", &ProjectFileParser::parseTranslationFile));
}

ProjectFileParser::~ProjectFileParser()
{
}

void ProjectFileParser::parse(const ProjectPtr & project, const std::string & filename, bool resolveImports)
{
	ProjectFileParser parser(filename);
	parser.doParse(project, resolveImports);
}

void ProjectFileParser::parseFromCurrentDirectory(const ProjectPtr & project, bool resolveImports)
{
	parse(project, g_Config->projectFileName, resolveImports);
}

void ProjectFileParser::parseFromGit(const ProjectPtr & project, const std::string & name,
	const GitRepositoryPtr & repo, Platform::Type platform)
{
	std::string file = pathConcat(repo->path(), g_Config->projectFileName);

	std::string pathPrefix;
	if (isValidPathPrefix(name))
		pathPrefix = name;
	else
	{
		pathPrefix = repo->path();
		if (pathPrefix.length() > 0 && pathIsSeparator(pathPrefix[pathPrefix.length() - 1]))
			pathPrefix.resize(pathPrefix.length() - 1);
		pathPrefix = pathGetFileName(pathPrefix);
	}
	pathPrefix = pathConcat(".yip-imports", pathPrefix);

	ProjectFileParser parser(file, pathPrefix, platform);
	parser.doParse(project, true);
}

void ProjectFileParser::parseFromGit(const ProjectPtr & project, const std::string & url, Platform::Type platform)
{
	GitRepositoryPtr repo;
	try {
		repo = project->yipDirectory()->openGitRepository(url);
	} catch (const std::exception & e) {
		throw std::runtime_error(fmt() << "unable to open git repository at '" << url << "': " << e.what());
	}
	parseFromGit(project, url, repo, platform);
}

void ProjectFileParser::reportWarning(const std::string & message)
{
	std::cerr << m_FileName << '(' << m_TokenLine << "): " << message << std::endl;
	m_Project->setValid(false);
}

void ProjectFileParser::reportError(const std::string & message)
{
	throw Error(fmt() << m_FileName << '(' << m_TokenLine << "): " << message);
}

void ProjectFileParser::doParse(const ProjectPtr & project, bool resolveImports)
{
	time_t modificationTime = pathGetModificationTime(m_FileName);
	if (!project->hasModificationTime() || modificationTime > project->modificationTime())
		project->setModificationTime(modificationTime);

	m_Project = project.get();
	m_ResolveImports = resolveImports;

	for (;;)
	{
		switch (getToken())
		{
		case Token::Eof:
			break;

		case Token::Literal: {
			auto it = m_CommandHandlers.find(m_TokenText);
			if (it != m_CommandHandlers.end())
			{
				try {
					(this->*(it->second))();
				} catch (const Error &) {
					throw;
				} catch (const std::exception & e) {
					reportError(e.what());
					return;
				}
				continue;
			}
			goto unexpected;
			}

		default:
		unexpected:
			reportError(fmt() << "unexpected '" << m_TokenText << "'.");
			return;
		}
		break;
	}
}

void ProjectFileParser::parseProjectName()
{
	if (getToken() != Token::Literal)
		reportError(fmt() << "expected project name after 'project_name'.");
	else
		m_Project->setProjectName(m_TokenText);
}

void ProjectFileParser::parseSources()
{
	Platform::Type platforms = m_DefaultPlatformMask;
	if (getToken() == Token::Colon)
	{
		getToken();
		platforms = parsePlatformMask();
	}

	if (m_Token != Token::LCurly)
		reportError("expected '{'.");

	getToken();
	while (m_Token != Token::RCurly && m_Token != Token::Eof)
	{
		if (m_Token != Token::Literal)
			reportError("expected file name.");

		std::string name = m_TokenText;
		std::string path = pathMakeAbsolute(m_TokenText, m_ProjectPath);
		if (m_PathPrefix.length() > 0)
			name = pathConcat(m_PathPrefix, name);

		SourceFilePtr sourceFile;
		try {
			sourceFile = m_Project->addSourceFile(name, path);
			sourceFile->setPlatforms(platforms);
		} catch (const Error &) {
			throw;
		} catch (const std::exception & e) {
			reportWarning(e.what());
		}

		getToken();
		parseFileFlags(sourceFile);
	}

	if (m_Token != Token::RCurly)
		reportError("expected '}'.");
}

void ProjectFileParser::parseAppSources()
{
	Platform::Type platforms = m_DefaultPlatformMask;
	if (getToken() == Token::Colon)
	{
		getToken();
		platforms = parsePlatformMask();
	}

	if (m_Token != Token::LCurly)
		reportError("expected '{'.");

	getToken();
	while (m_Token != Token::RCurly && m_Token != Token::Eof)
	{
		if (m_Token != Token::Literal)
			reportError("expected file name.");

		SourceFilePtr sourceFile;
		if (m_PathPrefix.length() == 0)
		{
			std::string name = m_TokenText;
			std::string path = pathMakeAbsolute(m_TokenText, m_ProjectPath);
			if (m_PathPrefix.length() > 0)
				name = pathConcat(m_PathPrefix, name);

			try {
				sourceFile = m_Project->addSourceFile(name, path);
				sourceFile->setPlatforms(platforms);
			} catch (const Error &) {
				throw;
			} catch (const std::exception & e) {
				reportWarning(e.what());
			}
		}

		getToken();
		parseFileFlags(sourceFile);
	}

	if (m_Token != Token::RCurly)
		reportError("expected '}'.");
}

void ProjectFileParser::parsePublicHeaders()
{
	if (getToken() != Token::LCurly)
		reportError("expected '{'.");

	getToken();
	while (m_Token != Token::RCurly && m_Token != Token::Eof)
	{
		if (m_Token != Token::Literal)
			reportError("expected file name.");

		std::string name = m_TokenText;
		std::string path = pathMakeAbsolute(m_TokenText, m_ProjectPath);

		SourceFilePtr sourceFile;
		try {
			sourceFile = m_Project->addSourceFile(pathConcat(m_PathPrefix, name), path);
		} catch (const Error &) {
			throw;
		} catch (const std::exception & e) {
			reportWarning(e.what());
		}

		SourceFilePtr sourceFile2;
		if (m_PathPrefix.length() > 0)
		{
			std::string proxyName = pathConcat(".yip-import-proxies/yip-imports", name);
			std::string proxyPath = m_Project->yipDirectory()->writeIncludeWrapper(proxyName, path);
			try {
				sourceFile2 = m_Project->addSourceFile(pathConcat(".yip-imports-proxies", name), proxyPath);
			} catch (const Error &) {
				throw;
			} catch (const std::exception & e) {
				reportWarning(e.what());
			}
		}

		getToken();
		parseFileFlags(sourceFile, sourceFile2, true);
	}

	if (m_Token != Token::RCurly)
		reportError("expected '}'.");
}

void ProjectFileParser::parseDefines()
{
	BuildType::Value buildTypes = BuildType::All;
	Platform::Type platforms = m_DefaultPlatformMask;

	if (getToken() == Token::Colon)
	{
		getToken();
		parsePlatformOrBuildTypeMask(platforms, buildTypes);
	}

	if (m_Token != Token::LCurly)
		reportError("expected '{'.");

	getToken();
	while (m_Token != Token::RCurly && m_Token != Token::Eof)
	{
		if (m_Token != Token::Literal)
			reportError("expected preprocessor definition.");
		m_Project->addDefine(m_TokenText, platforms, buildTypes);
		getToken();
	}

	if (m_Token != Token::RCurly)
		reportError("expected '}'.");
}

void ProjectFileParser::parseAppDefines()
{
	BuildType::Value buildTypes = BuildType::All;
	Platform::Type platforms = m_DefaultPlatformMask;

	if (getToken() == Token::Colon)
	{
		getToken();
		parsePlatformOrBuildTypeMask(platforms, buildTypes);
	}

	if (m_Token != Token::LCurly)
		reportError("expected '{'.");

	getToken();
	while (m_Token != Token::RCurly && m_Token != Token::Eof)
	{
		if (m_Token != Token::Literal)
			reportError("expected preprocessor definition.");

		if (m_PathPrefix.length() == 0)
			m_Project->addDefine(m_TokenText, platforms, buildTypes);

		getToken();
	}

	if (m_Token != Token::RCurly)
		reportError("expected '}'.");
}

void ProjectFileParser::parseImport()
{
	if (getToken() != Token::Literal)
		reportError("expected dependency name after 'import'.");

	auto it = g_Config->repos.find(m_TokenText);
	std::string url = (it != g_Config->repos.end() ? it->second : m_TokenText);
	std::string name = (it != g_Config->repos.end() ? it->first : m_TokenText);

	if (!m_Project->addImport(url))
		return;

	if (!m_ResolveImports)
		return;

	GitRepositoryPtr repo;
	try {
		repo = m_Project->yipDirectory()->openGitRepository(url);
	} catch (const std::exception & e) {
		throw std::runtime_error(fmt() << "unable to open git repository at '" << url << "': " << e.what());
	}

	try {
		ProjectFileParser::parseFromGit(m_Project->shared_from_this(), name, repo);
	} catch (const std::exception & e) {
		reportWarning(fmt() << "unable to parse project file in git repository at '" << url << "': " << e.what());
	}
}

void ProjectFileParser::parseResources()
{
	Platform::Type platforms = m_DefaultPlatformMask;
	if (getToken() == Token::Colon)
	{
		getToken();
		platforms = parsePlatformMask();
	}

	if (m_Token != Token::LCurly)
		{ reportError("expected '{'."); return; }

	getToken();
	while (m_Token != Token::RCurly && m_Token != Token::Eof)
	{
		if (m_Token != Token::Literal)
			{ reportError("expected file name."); return; }

		std::string name = m_TokenText;
		std::string path = pathMakeAbsolute(name, m_ProjectPath);

		try {
			SourceFilePtr sourceFile = m_Project->addResourceFile(name, path);
			sourceFile->setPlatforms(platforms);
		} catch (const Error &) {
			throw;
		} catch (const std::exception & e) {
			reportWarning(e.what());
		}

		getToken();
	}

	if (m_Token != Token::RCurly)
		reportError("expected '}'.");
}

void ProjectFileParser::parseResourcesDir()
{
	Platform::Type platforms = m_DefaultPlatformMask;
	if (getToken() == Token::Colon)
	{
		getToken();
		platforms = parsePlatformMask();
	}

	if (m_Token != Token::Literal)
		{ reportError("expected directory name."); return; }

	std::string name = m_TokenText;
	std::string path = pathMakeAbsolute(name, m_ProjectPath);

	std::function<void(const std::string &, const std::string &)> processDir =
		[&processDir, platforms, this](const std::string & fullname, const std::string & fullpath)
	{
		DirEntryList list = pathEnumDirectoryContents(fullpath);
		for (auto it : list)
		{
			std::string resName = pathConcat(fullname, it.name);
			std::string resPath = pathConcat(fullpath, it.name);

			if (it.type == DirEntry_Directory)
			{
				processDir(resName, resPath);
				continue;
			}

			try {
				SourceFilePtr sourceFile = m_Project->addResourceFile(resName, resPath);
				sourceFile->setPlatforms(platforms);
			} catch (const Error &) {
				throw;
			} catch (const std::exception & e) {
				reportWarning(e.what());
			}
		}
	};

	processDir(name, path);
}

void ProjectFileParser::parseAppResources()
{
	Platform::Type platforms = m_DefaultPlatformMask;
	if (getToken() == Token::Colon)
	{
		getToken();
		platforms = parsePlatformMask();
	}

	if (m_Token != Token::LCurly)
		{ reportError("expected '{'."); return; }

	getToken();
	while (m_Token != Token::RCurly && m_Token != Token::Eof)
	{
		if (m_Token != Token::Literal)
			{ reportError("expected file name."); return; }

		std::string name = m_TokenText;
		std::string path = pathMakeAbsolute(name, m_ProjectPath);

		if (m_PathPrefix.length() == 0)
		{
			try {
				SourceFilePtr sourceFile = m_Project->addResourceFile(name, path);
				sourceFile->setPlatforms(platforms);
			} catch (const Error &) {
				throw;
			} catch (const std::exception & e) {
				reportWarning(e.what());
			}
		}

		getToken();
	}

	if (m_Token != Token::RCurly)
		reportError("expected '}'.");
}

void ProjectFileParser::parseWinRT()
{
	std::string prefix = m_TokenText;

	if (getToken() != Token::Colon)
		{ reportError(fmt() << "expected ':' after '" << prefix << "'."); return; }

	if (getToken() != Token::Literal)
		{ reportError(fmt() << "expected variable name after '" << prefix << ":'."); return; }

	if (m_TokenText == "library")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected library name after '" << prefix << ":library'."); return; }
		m_Project->winrtAddLibrary(m_TokenText);
		return;
	}

	reportError(fmt() << "invalid variable '" << prefix << ":" << m_TokenText << "'.");
}

void ProjectFileParser::parseIOSorOSX()
{
	std::string prefix = m_TokenText;
	bool iOS = (prefix == "ios");

	if (getToken() != Token::Colon)
		{ reportError(fmt() << "expected ':' after '" << prefix << "'."); return; }

	if (getToken() != Token::Literal)
		{ reportError(fmt() << "expected variable name after '" << prefix << ":'."); return; }

	if (m_TokenText == "framework")
	{
		std::string name, path;
		if (getToken() != Token::LParen)
		{
			if (m_Token != Token::Literal)
				{ reportError(fmt() << "expected framework name after '" << prefix << ":framework'."); return; }
			name = m_TokenText;
			if (pathGetShortFileExtension(name) == "")
				name += ".framework";
		}
		else
		{
			if (getToken() != Token::Literal)
				{ reportError("expected framework name after '('."); return; }
			name = m_TokenText;
			if (getToken() != Token::Arrow)
				{ reportError("expected '=>'."); return; }
			if (getToken() != Token::Literal)
				{ reportError(fmt() << "expected framework path after '=>'."); return; }
			path = m_TokenText;
			if (getToken() != Token::RParen)
				{ reportError("expected ')'."); return; }

			if (path.length() < 2 || path[0] != '$' || !pathIsSeparator(path[1]))
				path = pathMakeAbsolute(path, m_ProjectPath);
		}

		try
		{
			if (iOS)
				m_Project->iosAddFramework(name, path);
			else
				m_Project->osxAddFramework(name, path);
		}
		catch (const Error &)
		{
			throw;
		}
		catch (const std::exception & e)
		{
			reportWarning(e.what());
		}

		return;
	}
	else if (m_TokenText == "deployment_target")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected deployment target after '" << prefix << ":deployment_target'."); return; }

		if (iOS)
			m_Project->iosSetDeploymentTarget(m_TokenText);
		else
			m_Project->osxSetDeploymentTarget(m_TokenText);

		return;
	}
	else if (m_TokenText == "icon")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected icon path after '" << prefix << ":icon'."); return; }

		std::string path = pathMakeAbsolute(m_TokenText, m_ProjectPath);

		unsigned width = 0, height = 0;
		ImageFormat format;
		imageGetInfo(path, &format, &width, &height);

		if (format != FORMAT_PNG)
			{ reportError(fmt() << "file '" << path << "' is not png."); return; }

		Project::ImageSize imageSize = Project::IMAGESIZE_INVALID;
		if (!iOS)
			{ reportError(fmt() << "osx:icon is not supported yet."); return; }		// FIXME
		else
		{
			imageSize = validateImageSize(width, height, {
				{ Project::IMAGESIZE_IPHONE_STANDARD,		57,		57 },
				{ Project::IMAGESIZE_IPAD_STANDARD_IOS6,	72,		72 },
				{ Project::IMAGESIZE_IPAD_STANDARD,			76,		76 },
				{ Project::IMAGESIZE_IPHONE_RETINA_IOS6,	114,	114 },
				{ Project::IMAGESIZE_IPHONE_RETINA,			120,	120 },
				{ Project::IMAGESIZE_IPAD_RETINA_IOS6,		144,	144 },
				{ Project::IMAGESIZE_IPAD_RETINA,			152,	152 },
			});
		}

		if (imageSize == Project::IMAGESIZE_INVALID)
			return;

		try
		{
			if (iOS)
				m_Project->iosAddIcon(imageSize, path);
			else
				m_Project->osxAddIcon(imageSize, path);
		}
		catch (const Error &)
		{
			throw;
		}
		catch (const std::exception & e)
		{
			reportWarning(e.what());
		}

		return;
	}
	else if (m_TokenText == "launch_image" && iOS)
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected launch image path after '" << prefix << ":launch_image'."); return; }

		std::string path = pathMakeAbsolute(m_TokenText, m_ProjectPath);

		unsigned width = 0, height = 0;
		ImageFormat format;
		imageGetInfo(path, &format, &width, &height);

		if (format != FORMAT_PNG)
			{ reportError(fmt() << "file '" << path << "' is not png."); return; }

		Project::ImageSize imageSize = Project::IMAGESIZE_INVALID;
		imageSize = validateImageSize(width, height, {
			{ Project::IMAGESIZE_LAUNCH_IPHONE_STANDARD,		320,	480 },
			{ Project::IMAGESIZE_LAUNCH_IPHONE_RETINA,			640,	960 },
			{ Project::IMAGESIZE_LAUNCH_IPHONE5_RETINA,			640,	1136 },
			{ Project::IMAGESIZE_LAUNCH_IPAD_PORTRAIT,			768,	1024 },
			{ Project::IMAGESIZE_LAUNCH_IPAD_PORTRAIT_RETINA,	1536,	2048 },
			{ Project::IMAGESIZE_LAUNCH_IPAD_LANDSCAPE,			1024,	768 },
			{ Project::IMAGESIZE_LAUNCH_IPAD_LANDSCAPE_RETINA,	2048,	1536 },
		});

		if (imageSize == Project::IMAGESIZE_INVALID)
			return;

		try {
			m_Project->iosAddLaunchImage(imageSize, path);
		} catch (const Error &) {
			throw;
		} catch (const std::exception & e) {
			reportWarning(e.what());
		}

		return;
	}
	else if (m_TokenText == "font" && iOS)
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected font path after '" << prefix << ":font'."); return; }

		m_Project->iosAddFont(m_TokenText);

		return;
	}
	else if (m_TokenText == "bundle_id")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected bundle identifier after '" << prefix << ":bundle_id'."); return; }

		if (iOS)
			m_Project->iosSetBundleIdentifier(m_TokenText);
		else
			m_Project->osxSetBundleIdentifier(m_TokenText);

		return;
	}
	else if (m_TokenText == "bundle_ver")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected bundle version after '" << prefix << ":bundle_ver'."); return; }

		if (iOS)
			m_Project->iosSetBundleVersion(m_TokenText);
		else
			m_Project->osxSetBundleVersion(m_TokenText);

		return;
	}
	else if (m_TokenText == "display_name" && iOS)
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected display name after '" << prefix << ":display_name'."); return; }
		m_Project->iosSetBundleDisplayName(m_TokenText);
		return;
	}
	else if (m_TokenText == "supported_devices" && iOS)
	{
		m_Project->iosSetAllowIPad(false);
		m_Project->iosSetAllowIPhone(false);

		if (getToken() != Token::LParen)
			{ reportError("expected '('."); return; }

		for (;;)
		{
			if (getToken() != Token::Literal)
				{ reportError("expected device family name."); return; }

			if (m_TokenText == "iphone")
				m_Project->iosSetAllowIPhone(true);
			else if (m_TokenText == "ipad")
				m_Project->iosSetAllowIPad(true);
			else
				{ reportError(fmt() << "invalid device family name '" << m_TokenText << "'."); }

			switch (getToken())
			{
			case Token::RParen:
				break;
			case Token::Comma:
				continue;
			default:
				reportError("expected ')'.");
				return;
			}
			break;
		}

		return;
	}
	else if (m_TokenText == "facebook_app_id" && iOS)
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected literal after '" << prefix << ":facebook_app_id'."); return; }

		m_Project->iosSetFacebookAppID(m_TokenText);

		return;
	}
	else if (m_TokenText == "facebook_display_name" && iOS)
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected literal after '" << prefix << ":facebook_display_name'."); return; }

		m_Project->iosSetFacebookDisplayName(m_TokenText);

		return;
	}
	else if (m_TokenText == "vk_app_id" && iOS)
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected literal after '" << prefix << ":vk_app_id'."); return; }

		m_Project->iosSetVkAppID(m_TokenText);

		return;
	}
	else if (m_TokenText == "view_controller" && iOS)
	{
		Project::IOSViewController cntrl;

		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected view controller name after '" << prefix << ":view_controller'."); return; }
		cntrl.name = m_TokenText;

		if (getToken() != Token::Literal)
			cntrl.parentClass = "UIViewController";
		else
		{
			cntrl.parentClass = m_TokenText;
			getToken();
		}

		if (m_Token == Token::Arrow)
		{
			if (getToken() != Token::Literal)
				{ reportError("expected XML file name after '=>'."); return; }

			std::string name = m_TokenText;
			std::string path = pathMakeAbsolute(name, m_ProjectPath);

			try {
				cntrl.iphone = m_Project->addUILayoutFile(name, path, Platform::iOS);
				cntrl.ipad = m_Project->addUILayoutFile(name, path, Platform::iOS);
			} catch (const Error &) {
				throw;
			} catch (const std::exception & e) {
				reportWarning(e.what());
			}
		}
		else
		{
			if (m_Token != Token::LCurly)
				{ reportError("expected '{'."); return; }

			for (;;)
			{
				if (getToken() != Token::Literal)
					{ reportError("expected device family/orientation name."); return; }

				SourceFilePtr * target = nullptr;
				if (m_TokenText == "iphone")
					target = &cntrl.iphone;
				else if (m_TokenText == "ipad")
					target = &cntrl.ipad;
				else
					{ reportError(fmt() << "invalid device family/orientation name '" << m_TokenText << "'."); }

				if (target->get() != nullptr)
					{ reportError(fmt() << "duplicate family/orientation '" << m_TokenText << "'."); }

				if (getToken() != Token::Arrow)
					{ reportError("expected '=>'."); return; }

				if (getToken() != Token::Literal)
					{ reportError("expected XML file name after '=>'."); return; }

				std::string name = m_TokenText;
				std::string path = pathMakeAbsolute(name, m_ProjectPath);

				try {
					*target = m_Project->addUILayoutFile(name, path, Platform::iOS);
				} catch (const Error &) {
					throw;
				} catch (const std::exception & e) {
					reportWarning(e.what());
				}

				switch (getToken())
				{
				case Token::RCurly:
					break;
				case Token::Comma:
					continue;
				default:
					reportError("expected '}'.");
					return;
				}
				break;
			}
		}

		try {
			m_Project->iosAddViewController(cntrl);
			m_Project->setShouldImportIOSUtil();
		} catch (const Error &) {
			throw;
		} catch (const std::exception & e) {
			reportWarning(e.what());
		}

		return;
	}

	reportError(fmt() << "invalid variable '" << prefix << ":" << m_TokenText << "'.");
}

void ProjectFileParser::parseAndroid()
{
	std::string prefix = m_TokenText;

	if (getToken() != Token::Colon)
		{ reportError(fmt() << "expected ':' after '" << prefix << "'."); return; }

	if (getToken() != Token::Literal)
		{ reportError(fmt() << "expected variable name after '" << prefix << ":'."); return; }

	if (m_TokenText == "target")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected target name after '" << prefix << ":target'."); return; }
		m_Project->androidSetTarget(m_TokenText);
		return;
	}
	else if (m_TokenText == "package")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected package name after '" << prefix << ":package'."); return; }
		m_Project->androidSetPackage(m_TokenText);
		return;
	}
	else if (m_TokenText == "display_name")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected display name after '" << prefix << ":display_name'."); return; }
		m_Project->androidSetDisplayName(m_TokenText);
		return;
	}
	else if (m_TokenText == "gles_version")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected opengl version after '" << prefix << ":gles_version'."); return; }
		m_Project->androidSetGlEsVersion(m_TokenText);
		return;
	}
	else if (m_TokenText == "nativelib")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected library name after '" << prefix << ":nativelib'."); return; }
		m_Project->androidAddNativeLib(m_TokenText);
		return;
	}
	else if (m_TokenText == "min_sdk_version")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected minimum SDK version after '" << prefix << ":min_sdk_version'."); return; }
		const char * p = m_TokenText.c_str(), * end = nullptr;
		long value = strtol(p, (char **)&end, 10);
		if (end != p + m_TokenText.length() || value <= 0 || value > 1000)
			{ reportError(fmt() << "invalid value for '" << prefix << ":min_sdk_version'."); return; }
		m_Project->androidSetMinSdkVersion(int(value));
		return;
	}
	else if (m_TokenText == "target_sdk_version")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected target SDK version after '" << prefix << ":target_sdk_version'."); return; }
		const char * p = m_TokenText.c_str(), * end = nullptr;
		long value = strtol(p, (char **)&end, 10);
		if (end != p + m_TokenText.length() || value <= 0 || value > 1000)
			{ reportError(fmt() << "invalid value for '" << prefix << ":target_sdk_version'."); return; }
		m_Project->androidSetTargetSdkVersion(int(value));
		return;
	}
	else if (m_TokenText == "manifest_activity")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected activity XML after '" << prefix << ":manifest_activity'."); return; }
		m_Project->androidAddManifestActivity(m_TokenText);
		return;
	}
	else if (m_TokenText == "java_srcdir")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected path to the Java source dir after '" << prefix << ":java_srcdir'."); return; }
		m_Project->androidAddJavaSourceDir(pathMakeAbsolute(m_TokenText, m_ProjectPath));
		return;
	}
	else if (m_TokenText == "make_activity")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected class name after '" << prefix << ":make_activity'."); return; }
		std::string name = m_TokenText;
		if (getToken() != Token::Arrow)
			{ reportError("expected '=>'."); return; }
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected class name after '=>'."); return; }
		std::string parentClass = m_TokenText;
		if (!m_Project->androidAddMakeActivity(name, parentClass))
			reportWarning(fmt() << "duplicate 'make_activity' for class '" << name << "'.");
		return;
	}
	else if (m_TokenText == "view")
	{
		Project::AndroidView view;

		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected view name after '" << prefix << ":view'."); return; }
		view.name = m_TokenText;

		if (getToken() != Token::LCurly)
			{ reportError("expected '{'."); return; }
		for (;;)
		{
			if (getToken() != Token::Literal)
				{ reportError("expected device family/orientation name."); return; }

			SourceFilePtr * target = nullptr;
			if (m_TokenText == "phone")
				target = &view.phone;
			else if (m_TokenText == "tablet7")
				target = &view.tablet7;
			else if (m_TokenText == "tablet10")
				target = &view.tablet10;
			else
				{ reportError(fmt() << "invalid device family/orientation name '" << m_TokenText << "'."); }

			if (target->get() != nullptr)
				{ reportError(fmt() << "duplicate family/orientation '" << m_TokenText << "'."); }

			if (getToken() != Token::Arrow)
				{ reportError("expected '=>'."); return; }

			if (getToken() != Token::Literal)
				{ reportError("expected XML file name."); return; }

			std::string name = m_TokenText;
			std::string path = pathMakeAbsolute(name, m_ProjectPath);

			try {
				*target = m_Project->addUILayoutFile(name, path, Platform::Android);
			} catch (const Error &) {
				throw;
			} catch (const std::exception & e) {
				reportWarning(e.what());
			}

			switch (getToken())
			{
			case Token::RCurly:
				break;
			case Token::Comma:
				continue;
			default:
				reportError("expected '}'.");
				return;
			}
			break;
		}

		try {
			m_Project->androidAddView(view);
			m_Project->setShouldImportAndroidUtil();
		} catch (const Error &) {
			throw;
		} catch (const std::exception & e) {
			reportWarning(e.what());
		}

		return;
	}
	else if (m_TokenText == "icon")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected icon path after '" << prefix << ":icon'."); return; }

		std::string path = pathMakeAbsolute(m_TokenText, m_ProjectPath);

		unsigned width = 0, height = 0;
		ImageFormat format;
		imageGetInfo(path, &format, &width, &height);

		if (format != FORMAT_PNG)
			{ reportError(fmt() << "file '" << path << "' is not png."); return; }

		Project::ImageSize imageSize = validateImageSize(width, height, {
			{ Project::IMAGESIZE_ANDROID_LDPI,			36,		36 },
			{ Project::IMAGESIZE_ANDROID_MDPI,			48,		48 },
			{ Project::IMAGESIZE_ANDROID_HDPI,			72,		72 },
			{ Project::IMAGESIZE_ANDROID_XHDPI,			96,		96 },
			{ Project::IMAGESIZE_ANDROID_XXHDPI,		144,	144 },
			{ Project::IMAGESIZE_ANDROID_XXXHDPI,		192,	192 },
		});

		if (imageSize == Project::IMAGESIZE_INVALID)
			return;

		try {
			m_Project->androidAddIcon(imageSize, path);
		} catch (const Error &) {
			throw;
		} catch (const std::exception & e) {
			reportWarning(e.what());
		}

		return;
	}

	reportError(fmt() << "invalid variable '" << prefix << ":" << m_TokenText << "'.");
}

void ProjectFileParser::parseTizen()
{
	std::string prefix = m_TokenText;

	if (getToken() != Token::Colon)
		{ reportError(fmt() << "expected ':' after '" << prefix << "'."); return; }

	if (getToken() != Token::Literal)
		{ reportError(fmt() << "expected variable name after '" << prefix << ":'."); return; }

	if (m_TokenText == "privilege")
	{
		if (getToken() != Token::Literal)
			{ reportError(fmt() << "expected privilege name after '" << prefix << ":privilege'."); return; }
		m_Project->tizenAddPrivilege(m_TokenText);
		return;
	}

	reportError(fmt() << "invalid variable '" << prefix << ":" << m_TokenText << "'.");
}

void ProjectFileParser::parseLicense()
{
	if (getToken() != Token::Literal)
		reportError("expected license text after 'license'.");
	m_Project->addLicense(m_TokenText);
}

void ProjectFileParser::parseToDo()
{
	if (getToken() != Token::LCurly)
		reportError("expected '{'.");

	if (getToken() == Token::RCurly)
		return;

	for (;;)
	{
		if (m_Token != Token::Literal)
			{ reportError("expected todo message."); return; }
		std::string message = m_TokenText;

		int year = -1, month = -1, day = -1;
		if (getToken() == Token::Literal)
		{
			if (m_TokenText != "before")
				{ reportError("expected 'before' or ','."); return; }

			if (getToken() != Token::Literal)
				{ reportError("expected date after 'before'."); return; }

			if (sscanf(m_TokenText.c_str(), "%d-%d-%d", &year, &month, &day) != 3
					|| year < 0 || month < 0 || day < 0)
				{ reportError(fmt() << "invalid date '" << m_TokenText << "'."); return; }

			getToken();
		}

		m_Project->addToDo(m_FileName, m_TokenLine, message, year, month, day);

		if (m_Token == Token::Comma)
			getToken();
		else if (m_Token == Token::RCurly)
			break;
		else
			{ reportError("expected '}'."); return; }
	}
}

void ProjectFileParser::parseTranslationFile()
{
	if (getToken() != Token::Literal)
		{ reportError("expected language name after 'translation_file'."); return; }
	std::string language = m_TokenText;

	if (getToken() != Token::Literal)
		{ reportError("expected translation file name."); return; }
	std::string file = m_TokenText;

	try
	{
		std::string path = pathMakeAbsolute(file, m_ProjectPath);
		m_Project->addTranslationFile(language, file, path);
	}
	catch (const std::exception & e)
	{
		reportError(e.what());
	}
}

Platform::Type ProjectFileParser::parsePlatformMask()
{
	bool inverse = false;
	if (m_Token == Token::Exclamation)
	{
		inverse = true;
		getToken();
	}

	Platform::Type mask = 0;
	for (;;)
	{
		mask |= parsePlatformName();
		if (m_Token != Token::Comma)
			break;
		getToken();
	}

	return (inverse ? ~mask : mask);
}

Platform::Type ProjectFileParser::parsePlatformName()
{
	if (m_Token != Token::Literal)
	{
		reportError("expected platform name.");
		return Platform::None;
	}

	Platform::Type result = platformFromString(m_TokenText);
	getToken();

	return result;
}

void ProjectFileParser::parsePlatformOrBuildTypeMask(Platform::Type & platforms, BuildType::Value & buildTypes)
{
	BuildType::Value buildMask = 0;
	Platform::Type platfMask = 0;
	bool inverse = false;

	if (m_Token == Token::Exclamation)
	{
		inverse = true;
		getToken();
	}

	bool hasBuildTypes = false, hasPlatforms = false;
	for (;;)
	{
		if (m_Token != Token::Literal)
		{
			reportError("expected platform or build type name.");
			return;
		}

		BuildType::Value buildType = buildTypeFromString(m_TokenText, std::nothrow);
		if (buildType != BuildType::Unspecified)
		{
			buildMask |= buildType;
			hasBuildTypes = true;
		}
		else
		{
			platfMask |= parsePlatformName();
			hasPlatforms = true;
		}

		if (m_Token != Token::Comma)
			break;

		getToken();
	}

	if (hasPlatforms)
		platforms = (inverse ? ~platfMask : platfMask);
	if (hasBuildTypes)
		buildTypes = (inverse ? ~buildMask : buildMask);
}

void ProjectFileParser::parseFileFlags(const SourceFilePtr & sourceFile, const SourceFilePtr & sourceFile2,
	bool isPublicHeader)
{
	(void)isPublicHeader;

	if (m_Token != Token::LCurly)
		return;

	getToken();
	while (m_Token != Token::RCurly && m_Token != Token::Eof)
	{
		if (m_Token != Token::Literal)
		{
			reportError("expected option name.");
			return;
		}
		std::string name = m_TokenText;

		if (getToken() != Token::Equal)
		{
			reportError("expected '='.");
			return;
		}

		if (getToken() != Token::Literal)
		{
			reportError("expected option value.");
			return;
		}
		std::string value = m_TokenText;

		if (name == "type")
		{
			FileType type = fileTypeFromString(value);
			if (type == FILE_UNKNOWN)
				reportWarning(fmt() << "invalid file type: '" << value << "'.");
			else
			{
				if (sourceFile)
					sourceFile->setFileType(type);
				if (sourceFile2)
					sourceFile2->setFileType(type);
			}
		}
		else if (name == "arc")
		{
			if (value == "yes")
			{
				if (sourceFile)
					sourceFile->setArcEnabled(true);
			}
			else if (value == "no")
			{
				if (sourceFile)
					sourceFile->setArcEnabled(false);
			}
			else
				reportWarning(fmt() << "invalid value '" << value << "' for option 'arc'.");
		}
		else
			reportWarning(fmt() << "invalid file option '" << name << "'.");

		if (getToken() == Token::Comma)
			getToken();
	}

	if (m_Token != Token::RCurly)
		reportError("expected '}'.");
	getToken();
}

ProjectFileParser::Token ProjectFileParser::getToken()
{
	int quote;

	#define DIGITS \
			 '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'

	#define LETTERS \
			 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': \
		case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': \
		case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': case 'A': case 'B': case 'C': case 'D': \
		case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': \
		case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': \
		case 'Y': case 'Z'

	#define EXTRA_SYMBOLS \
			 '_': case '-': case '+': case '/': case '.': case '~'

	m_TokenText.clear();
	m_Token = Token::Eof;
	m_TokenLine = m_CurLine;

	for (;;)
	{
		int ch = getChar();
		switch (ch)
		{
		case EOF:
			return (m_Token = Token::Eof);

		case ' ':
		case '\t':
		case '\r':
		case '\n':
		case '\v':
		case '\f':
			m_TokenLine = m_CurLine;
			continue;

		case '{':
			return (m_TokenText = '{', m_Token = Token::LCurly);

		case '}':
			return (m_TokenText = '}', m_Token = Token::RCurly);

		case '(':
			return (m_TokenText = '(', m_Token = Token::LParen);

		case ')':
			return (m_TokenText = ')', m_Token = Token::RParen);

		case ':':
			return (m_TokenText = ':', m_Token = Token::Colon);

		case '!':
			return (m_TokenText = '!', m_Token = Token::Exclamation);

		case ',':
			return (m_TokenText = ',', m_Token = Token::Comma);

		case '=':
			ch = getChar();
			if (ch == '>')
				return (m_TokenText = "=>", m_Token = Token::Arrow);
			ungetChar();
			return (m_TokenText = '=', m_Token = Token::Equal);

		case LETTERS:
		case EXTRA_SYMBOLS:
			m_Buffer.str(std::string());
			for (;;)
			{
				m_Buffer << static_cast<char>(ch);
				ch = getChar();
				switch (ch)
				{
				case LETTERS:
				case DIGITS:
				case EXTRA_SYMBOLS:
					continue;
				case EOF:
					break;
				default:
					ungetChar();
				}
				break;
			}
			m_TokenText = m_Buffer.str();
			m_Buffer.str(std::string());
			return (m_Token = Token::Literal);

		case '"':
		case '\'':
		case '`':
			quote = ch;
			m_Buffer.str(std::string());
			for (;;)
			{
				ch = getChar();
				switch (ch)
				{
				case '"':
				case '\'':
				case '`':
					if (ch == quote)
						break;
					m_Buffer << static_cast<char>(ch);
					continue;
				case EOF:
					reportError("unterminated string literal.");
					return (m_Token = Token::Eof);
				case '\\':
					ch = getChar();
					switch (ch)
					{
					case 'n': m_Buffer << '\n'; continue;
					case '\\': m_Buffer << '\\'; continue;
					case '"': m_Buffer << '"'; continue;
					case '\'': m_Buffer << '\''; continue;
					case '`': m_Buffer << '`'; continue;
					default:
						reportError(fmt() << "invalid escape sequence '\\" << static_cast<char>(ch) << "'.");
						return (m_Token = Token::Eof);
					}
				default:
					m_Buffer << static_cast<char>(ch);
					continue;
				}
				break;
			}
			m_TokenText = m_Buffer.str();
			m_Buffer.str(std::string());
			return (m_Token = Token::Literal);

		default:
			reportError(fmt() << "unexpected symbol '" << static_cast<char>(ch) << "'.");
			return (m_Token = Token::Eof);
		}
	}
}

int ProjectFileParser::getChar()
{
	m_LastChar = m_Stream.get();
	if (m_LastChar == '\n')
		++m_CurLine;
	else if (m_LastChar == 0)
		throw std::runtime_error("attempted to parse a binary file.");
	return m_LastChar;
}

void ProjectFileParser::ungetChar()
{
	assert(m_LastChar != 0);
	m_Stream.unget();
	if (m_LastChar == '\n')
		--m_CurLine;
	m_LastChar = 0;
}

Project::ImageSize ProjectFileParser::validateImageSize(unsigned width, unsigned height,
	std::initializer_list<ImageSize> sizes)
{
	for (const ImageSize & size : sizes)
	{
		if (size.width == width && size.height == height)
			return size.size;
	}

	std::stringstream ss;
	ss << "invalid image size, valid sizes are:\n";
	for (const ImageSize & size : sizes)
		ss << "\t" << size.width << 'x' << size.height << '\n';
	reportError(ss.str());

	return Project::IMAGESIZE_INVALID;
}
