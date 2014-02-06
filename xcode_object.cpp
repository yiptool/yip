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
#include "xcode_object.h"
#include <cstring>
#include <sstream>

XCodeObject::XCodeObject(const std::string & className)
	: m_ClassName(className)
{
}

XCodeObject::~XCodeObject()
{
}

std::string XCodeObject::stringLiteral(const std::string & str)
{
	const char * allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_./";
	bool shortForm = true;

	if (str.length() == 0)
		return "\"\"";

	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!*it || !strchr(allowed, *it))
		{
			shortForm = false;
			break;
		}
	}

	if (shortForm)
		return str;

	std::stringstream ss;
	ss << '"';
	ss << str;
	ss << '"';

	return ss.str();
}

std::string XCodeObject::objectID(const XCodeObject * object)
{
	std::stringstream ss;

	ss << object->uniqueID().toString();

	const std::string & name = object->objectName();
	if (name.length() > 0)
		ss << " /* " << name << " */";

	return ss.str();
}
