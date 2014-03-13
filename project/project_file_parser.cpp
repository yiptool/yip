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
#include "../util/fmt.h"
#include "../util/path.h"
#include <cassert>
#include <stdexcept>

enum class ProjectFileParser::Token
{
	Eof = 0,
	LCurly,
	RCurly,
	Literal,
};

ProjectFileParser::ProjectFileParser(const std::string & filename, const ProjectConfigPtr & prjConfig)
	: m_ProjectConfig(prjConfig),
	  m_FileName(pathMakeAbsolute(filename)),
	  m_ProjectPath(pathGetDirectory(m_FileName)),
	  m_Token(Token::Eof),
	  m_CurLine(1),
	  m_TokenLine(1),
	  m_LastChar(0)
{
	m_Stream.open(filename, std::ios::in);
	if (!m_Stream.is_open() || m_Stream.fail() || m_Stream.bad())
		throw std::runtime_error(fmt() << "unable to open file '" << filename << "'.");

	if (!m_ProjectConfig)
		m_ProjectConfig = std::make_shared<ProjectConfig>(m_ProjectPath);

	m_CommandHandlers.insert(std::make_pair("sources", &ProjectFileParser::parseSources));
}

ProjectFileParser::~ProjectFileParser()
{
}

void ProjectFileParser::parse(const ProjectFilePtr & projectFile)
{
	m_ProjectFile = projectFile.get();

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
				(this->*(it->second))();
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

void ProjectFileParser::reportError(const std::string & message)
{
	throw std::runtime_error(fmt() << m_FileName << '(' << m_TokenLine << "):" << message);
}

void ProjectFileParser::parseSources()
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
		m_ProjectFile->addSourceFile(name, path);

		getToken();
	}

	if (m_Token != Token::RCurly)
		reportError("expected '}'.");
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
			 '_': case '-': case '+': case '/': case '.': case ',': case '~'

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
