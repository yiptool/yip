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
#ifndef __65633158e5f571c49679488619c5dda5__
#define __65633158e5f571c49679488619c5dda5__

#include "xcode_object.h"
#include <vector>

class XCodeProject;
class XCodeBuildConfiguration;

class XCodeConfigurationList : public XCodeObject
{
public:
	std::string objectName() const;

	inline const std::string & name() const { return m_Name; }
	inline void setName(const std::string & name_) { m_Name = name_; }

	inline bool defaultConfigurationIsVisible() const { return m_DefaultConfigurationIsVisible; }
	inline void setDefaultConfigurationIsVisible(bool flag) { m_DefaultConfigurationIsVisible = flag; }

	inline const std::string & defaultConfigurationName() const { return m_DefaultConfigurationName; }
	inline void setDefaultConfigurationName(const std::string & name_) { m_DefaultConfigurationName = name_; }

	inline void addConfiguration(const XCodeBuildConfiguration * cfg) { m_Cfgs.push_back(cfg); }

	std::string toString() const;

private:
	std::string m_Name;
	bool m_DefaultConfigurationIsVisible;
	std::string m_DefaultConfigurationName;
	std::vector<const XCodeBuildConfiguration *> m_Cfgs;

	XCodeConfigurationList();
	~XCodeConfigurationList();

	friend class XCodeProject;
};

#endif
