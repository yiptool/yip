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
#include "../util/fmt.h"
#include "../util/path.h"
#include <cassert>
#include <stdexcept>
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

	m_CommandHandlers.insert(std::make_pair("sources", &ProjectFileParser::parseSources));
	m_CommandHandlers.insert(std::make_pair("app_sources", &ProjectFileParser::parseAppSources));
	m_CommandHandlers.insert(std::make_pair("public_headers", &ProjectFileParser::parsePublicHeaders));
	m_CommandHandlers.insert(std::make_pair("defines", &ProjectFileParser::parseDefines));
	m_CommandHandlers.insert(std::make_pair("app_defines", &ProjectFileParser::parseAppDefines));
	m_CommandHandlers.insert(std::make_pair("import", &ProjectFileParser::parseImport));
	m_CommandHandlers.insert(std::make_pair("resources", &ProjectFileParser::parseResources));
	m_CommandHandlers.insert(std::make_pair("winrt", &ProjectFileParser::parseWinRT));
	m_CommandHandlers.insert(std::make_pair("ios", &ProjectFileParser::parseIOSorOSX));
	m_CommandHandlers.insert(std::make_pair("osx", &ProjectFileParser::parseIOSorOSX));
	m_CommandHandlers.insert(std::make_pair("tizen", &ProjectFileParser::parseTizen));
	m_CommandHandlers.insert(std::make_pair("license", &ProjectFileParser::parseLicense));
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
	throw std::runtime_error(fmt() << m_FileName << '(' << m_TokenLine << "): " << message);
}

void ProjectFileParser::doParse(const ProjectPtr & project, bool resolveImports)
{
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
				try
				{
					(this->*(it->second))();
				}
				catch (const std::exception & e)
				{
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
		try
		{
			sourceFile = m_Project->addSourceFile(name, path);
			sourceFile->setPlatforms(platforms);
		}
		catch (const std::exception & e)
		{
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

			try
			{
				sourceFile = m_Project->addSourceFile(name, path);
				sourceFile->setPlatforms(platforms);
			}
			catch (const std::exception & e)
			{
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

	if (!m_Project->addImport(name, url))
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

		try
		{
			SourceFilePtr sourceFile = m_Project->addResourceFile(name, path);
			sourceFile->setPlatforms(platforms);
		}
		catch (const std::exception & e)
		{
			reportWarning(e.what());
		}

		getToken();
	}

	if (m_Token != Token::RCurly)
		reportError("expected '}'.");
}

void ProjectFileParser::parseWinRT()
{
	const std::string & prefix = m_TokenText;

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
	const std::string & prefix = m_TokenText;
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
		catch (const std::exception & e)
		{
			reportWarning(e.what());
		}

		return;
	}

	reportError(fmt() << "invalid variable '" << prefix << ":" << m_TokenText << "'.");
}

void ProjectFileParser::parseTizen()
{
	const std::string & prefix = m_TokenText;

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
			m_Buffer.str(std::string());
			for (;;)
			{
				ch = getChar();
				switch (ch)
				{
				case '"':
					break;
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
