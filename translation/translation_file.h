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
#ifndef __232419b26a6a5538c9ca19e9a054e0ae__
#define __232419b26a6a5538c9ca19e9a054e0ae__

#include "../3rdparty/tinyxml/tinyxml.h"
#include <memory>
#include <map>
#include <string>

class Project;

class TranslationFile
{
public:
	TranslationFile(Project * prj, const std::string & lang, const std::string & name, const std::string & path);
	inline ~TranslationFile() {}

	inline const std::string & path() const { return m_Path; }

	void parse();
	void save() const;

	inline bool hasNonTranslatedStrings() const { return m_HasNonTranslatedStrings; }
	inline bool wasModified() const { return m_WasModified; }

	std::string getTranslation(const std::string & string) const;

private:
	std::string m_Language;
	std::string m_Name;
	std::string m_Path;
	Project * m_Project;
	mutable std::map<std::string, std::string> m_Strings;
	mutable bool m_HasXML;
	mutable bool m_HasNonTranslatedStrings;
	mutable bool m_WasModified;
	mutable TiXmlDocument m_XML;

	TranslationFile(const TranslationFile &) = delete;
	TranslationFile & operator=(const TranslationFile &) = delete;
};

typedef std::shared_ptr<TranslationFile> TranslationFilePtr;

#endif
