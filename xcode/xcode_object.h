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
#ifndef __b9c6e618163da94f7bc2235c732a4887__
#define __b9c6e618163da94f7bc2235c732a4887__

#include "xcode_unique_id.h"

class XCodeObject
{
public:
	inline const std::string & className() const { return m_ClassName; }
	virtual std::string objectName() const { return m_ClassName; }

	inline const XCodeUniqueID & uniqueID() const { return m_ID; }
	inline void setUniqueID(const XCodeUniqueID & id) { m_ID = id; }
	inline void setUniqueID(const std::string & id) { m_ID = id; }

	virtual std::string toString() const = 0;

protected:
	XCodeObject(const std::string & clsName);
	virtual ~XCodeObject();

	static std::string stringLiteral(const std::string & str);
	static std::string objectID(const XCodeObject * object);

private:
	XCodeUniqueID m_ID;
	std::string m_ClassName;
};

#endif
