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
#include <cassert>
#include <stdexcept>

enum class ProjectFileParser::Token
{
	Eof = 0,
	LCurly,
	RCurly,
	Identifier,
};

ProjectFileParser::ProjectFileParser(std::istream & stream, const std::string & filename)
	: m_Stream(stream),
	  m_FileName(filename),
	  m_Token(Token::Eof),
	  m_TokenLine(1),
	  m_LastChar(0)
{
}

ProjectFileParser::~ProjectFileParser()
{
}

void ProjectFileParser::parse()
{
	while (getToken() != Token::Eof)
		printf("%d [%s]\n", (int)m_Token, m_TokenText.c_str());
	printf("%d [%s]\n", (int)m_Token, m_TokenText.c_str());
}

void ProjectFileParser::reportError(const std::string & message)
{
	std::stringstream ss;
	ss << m_FileName << '(' << m_TokenLine << "):" << message;
	throw std::runtime_error(ss.str());
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
		case 'Y': case 'Z': case '_'

	m_TokenText.clear();
	m_Token = Token::Eof;

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
			continue;

		case '{':
			return (m_TokenText = '{', m_Token = Token::LCurly);

		case '}':
			return (m_TokenText = '}', m_Token = Token::RCurly);

		case LETTERS:
			m_Buffer.str(std::string());
			for (;;)
			{
				m_Buffer << static_cast<char>(ch);
				ch = getChar();
				switch (ch)
				{
				case LETTERS:
				case DIGITS:
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
			return (m_Token = Token::Identifier);

		default: {
			std::stringstream ss;
			ss << "unexpected symbol '" << static_cast<char>(ch) << "'.";
			reportError(ss.str());
			}
		}
	}
}

int ProjectFileParser::getChar()
{
	m_LastChar = m_Stream.get();
	if (m_LastChar == '\n')
		++m_TokenLine;
	else if (m_LastChar == 0)
		throw std::runtime_error("attempted to parse a binary file.");
	return m_LastChar;
}

void ProjectFileParser::ungetChar()
{
	assert(m_LastChar != 0);
	m_Stream.unget();
	if (m_LastChar == '\n')
		--m_TokenLine;
	m_LastChar = 0;
}
