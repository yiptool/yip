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
#include "translation_file.h"
#include "../project/project.h"
#include "../util/path-util/path-util.h"
#include "../util/tinyxml-util/tinyxml-util.h"
#include "../util/cxx-util/cxx-util/fmt.h"
#include "../util/cxx-util/cxx-util/write_file.h"
#include <sstream>
#include <cassert>

TranslationFile::TranslationFile(Project * prj, const std::string & lang, const std::string & name,
		const std::string & path)
	: m_Language(lang),
	  m_Name(name),
	  m_Path(pathMakeAbsolute(path)),
	  m_Project(prj),
	  m_HasXML(false),
	  m_HasNonTranslatedStrings(false),
	  m_WasModified(false)
{
}

void TranslationFile::parse()
{
	m_WasModified = false;

	try
	{
		if (pathIsFile(m_Path))
			std::cout << "parsing " << m_Name << std::endl;
		else
		{
			std::cout << "writing " << m_Name << std::endl;

			std::stringstream ss;
			ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
			ss << "<translations>\n";
			ss << "</translations>\n";

			writeFile(m_Path, ss.str());
		}

		m_HasXML = true;
		if (!m_XML.LoadFile(m_Path))
		{
			std::stringstream ss;
			ss << "error in '" << m_Path << "' at row " << m_XML.ErrorRow() << ", column "
				<< m_XML.ErrorCol() << ": " << m_XML.ErrorDesc();
			throw std::runtime_error(ss.str());
		}

		const TiXmlElement * root = xmlCheckRootElement(m_XML, "translations");
		for (const TiXmlElement * e = root->FirstChildElement(); e; e = e->NextSiblingElement())
		{
			if (e->ValueStr() != "string")
				throw std::runtime_error(xmlError(e, fmt() << "unexpected element '" << e->ValueStr() << "'."));

			const TiXmlAttribute * attrFrom = nullptr, * attrTo = nullptr;
			for (const TiXmlAttribute * attr = e->FirstAttribute(); attr; attr = attr->Next())
			{
				if (attr->NameTStr() == "f")
					attrFrom = attr;
				else if (attr->NameTStr() == "t")
					attrTo = attr;
				else
				{
					throw std::runtime_error(xmlError(attr,
						fmt() << "unexpected attribute '" << attr->NameTStr() << "'."));
				}
			}

			if (!attrFrom)
				throw std::runtime_error(xmlMissingAttribute(e, "f"));
			if (!attrTo)
				throw std::runtime_error(xmlMissingAttribute(e, "t"));

			const TiXmlElement * child = e->FirstChildElement();
			if (child)
			{
				throw std::runtime_error(xmlError(child,
					fmt() << "unexpected element '" << child->ValueStr() << "'."));
			}

			const std::string & from = attrFrom->ValueStr();
			std::string to = attrTo->ValueStr();

			if (to == "TBT")
			{
				m_Project->addToDo(m_Path, e->Row(), fmt() << "missing '" << m_Language
					<< "' translation for string '" << from << "'.");
				to = from;
				m_HasNonTranslatedStrings = true;
			}

			if (!m_Strings.insert(std::make_pair(from, to)).second)
			{
				throw std::runtime_error(xmlError(e,
					fmt() << "duplicate translation for string '" << from << "'."));
			}
		}
	}
	catch (...)
	{
		m_HasXML = false;
		m_HasNonTranslatedStrings = false;
		m_Strings.clear();
		throw;
	}
}

void TranslationFile::save() const
{
	if (m_HasXML)
	{
		std::cout << "writing " << m_Name << std::endl;
		m_XML.SaveFile(m_Path);
		m_WasModified = false;
	}
}

std::string TranslationFile::getTranslation(const std::string & string) const
{
	assert(m_HasXML);
	if (!m_HasXML)
		return string;

	auto it = m_Strings.find(string);
	if (it != m_Strings.end())
		return it->second;

	TiXmlElement * rootElement = xmlCheckRootElement(m_XML, "translations");
	m_Project->addToDo(m_Path, rootElement->Row(), fmt() << "missing '" << m_Language
		<< "' translation for string '" << string << "'.");

	TiXmlElement * newElement = new TiXmlElement("string");
	newElement->SetAttribute("f", string.c_str());
	newElement->SetAttribute("t", "TBT");
	rootElement->LinkEndChild(newElement);

	m_Strings.insert(std::make_pair(string, string));
	m_WasModified = true;
	m_HasNonTranslatedStrings = true;

	return string;
}
