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
#ifndef __1b931389685cae1a470191d95830fc3d__
#define __1b931389685cae1a470191d95830fc3d__

#include "platform.h"
#include <string>
#include <memory>

class Define
{
public:
	Define(const std::string & macroName);
	~Define();

	inline const std::string & name() const { return m_Name; }

	inline Platform::Type platforms() const { return m_Platforms; }
	inline void setPlatforms(Platform::Type flags) { m_Platforms = flags; }

	inline BuildType::Value buildTypes() const { return m_BuildTypes; }
	inline void setBuildTypes(BuildType::Value flags) { m_BuildTypes = flags; }

private:
	std::string m_Name;
	Platform::Type m_Platforms;
	BuildType::Value m_BuildTypes;

	Define(const Define &) = delete;
	Define & operator=(const Define &) = delete;
};

typedef std::shared_ptr<Define> DefinePtr;

#endif
