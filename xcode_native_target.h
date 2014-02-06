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
#ifndef __e7ffa55129417b512efd27d57612837d__
#define __e7ffa55129417b512efd27d57612837d__

#include "xcode_object.h"
#include <vector>

class XCodeProject;
class XCodeConfigurationList;
class XCodeBuildPhase;

extern const std::string PRODUCTTYPE_APPLICATION;
extern const std::string PRODUCTTYPE_TOOL;

class XCodeNativeTarget : public XCodeObject
{
public:
	inline std::string objectName() const { return m_Name; }

	inline const std::string & name() const { return m_Name; }
	inline void setName(const std::string & name) { m_Name = name; }

	inline const std::string & productName() const { return m_ProductName; }
	inline void setProductName(const std::string & name) { m_ProductName = name; }

	inline XCodeConfigurationList * buildConfigurationList() const { return m_BuildConfigurationList; }
	inline void setBuildConfigurationList(XCodeConfigurationList * cfg) { m_BuildConfigurationList = cfg; }

	inline XCodeObject * productReference() const { return m_ProductReference; }
	inline void setProductReference(XCodeObject * ref) { m_ProductReference = ref; }

	inline const std::string & productType() const { return m_ProductType; }
	inline void setProductType(const std::string & type) { m_ProductType = type; }

	inline void addBuildPhase(XCodeBuildPhase * phase) { m_Phases.push_back(phase); }

	std::string toString() const;

private:
	std::string m_Name;
	std::string m_ProductName;
	XCodeConfigurationList * m_BuildConfigurationList;
	XCodeObject * m_ProductReference;
	std::string m_ProductType;
	std::vector<XCodeBuildPhase *> m_Phases;

	XCodeNativeTarget();
	~XCodeNativeTarget();

	friend class XCodeProject;
};

#endif
