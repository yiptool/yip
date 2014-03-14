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
#include "xcode_unique_id.h"
#include "../util/fmt.h"
#include "../util/sha1.h"
#include <ctime>
#include <cstring>
#include <sstream>

static volatile uint32_t g_Counter;

uint32_t XCodeUniqueID::m_Seed[3];
bool XCodeUniqueID::m_HasSeed;

XCodeUniqueID::XCodeUniqueID()
{
	if (!m_HasSeed)
		setRandomSeed();

	static_assert(sizeof(m_ID) == sizeof(m_Seed), "sizeof(m_ID) == sizeof(m_Seed)");
	memcpy(m_ID, m_Seed, sizeof(m_ID));
	m_ID[2] += g_Counter++;
}

XCodeUniqueID::XCodeUniqueID(const std::string & hex)
{
	const char * p = hex.c_str();
	for (size_t i = 0; i < 3; i++)
	{
		unsigned val = 0;
		for (size_t j = 0; j < 32; j += 4)
		{
			char ch = *p;
			if (ch)
				++p;

			unsigned digit = 0;
			if (ch >= '0' && ch <= '9')
				digit = static_cast<unsigned>(ch - '0');
			else if (ch >= 'a' && ch <= 'f')
				digit = static_cast<unsigned>(ch - 'a' + 10);
			else if (ch >= 'A' && ch <= 'F')
				digit = static_cast<unsigned>(ch - 'A' + 10);

			val |= digit << j;
		}
		m_ID[i] = val;
	}
}

XCodeUniqueID::~XCodeUniqueID()
{
}

void XCodeUniqueID::setRandomSeed()
{
	m_Seed[0] = static_cast<uint32_t>(time(nullptr));
	m_Seed[1] = static_cast<uint32_t>(reinterpret_cast<size_t>(&m_HasSeed) & 0xFFFFFFFFU);
	m_Seed[2] = ((m_Seed[0] + m_Seed[1]) ^ 0xCAFEBABE);
	m_HasSeed = true;
}

void XCodeUniqueID::setSeed(const std::string & seed)
{
	std::string seed_sha1 = sha1(seed);
	const char * p = seed_sha1.c_str();

	for (size_t i = 0; i < 3; i++)
	{
		m_Seed[i] = 0;
		for (size_t j = 0; j < 32; j += 4)
		{
			char ch = *p++;
			if (ch >= '0' && ch <= '9')
				ch -= '0';
			else if (ch >= 'a' && ch <= 'f')
				ch = ch - 'a' + 10;
			else if (ch >= 'A' && ch <= 'F')
				ch = ch - 'A' + 10;
			m_Seed[i] |= (static_cast<unsigned>(ch) & 0xF) << j;
		}
	}

	m_HasSeed = true;
}

std::string XCodeUniqueID::toString() const
{
	std::stringstream ss;
	const char * hex = "0123456789ABCDEF";
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 32; j += 4)
			ss << hex[(m_ID[i] >> j) & 0xF];
	}
	return ss.str();
}
