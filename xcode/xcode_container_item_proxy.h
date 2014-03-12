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
#ifndef __bc492a198006cd01bfab6ef7c0959700__
#define __bc492a198006cd01bfab6ef7c0959700__

#include "xcode_object.h"
#include <vector>

class XCodeProject;

class XCodeContainerItemProxy : public XCodeObject
{
public:
	inline XCodeObject * containerPortal() const { return m_ContainerPortal; }
	inline void setContainerPortal(XCodeObject * portal) { m_ContainerPortal = portal; }

	inline const std::string & proxyType() const { return m_ProxyType; }
	inline void setProxyType(const std::string & type) { m_ProxyType = type; }

	inline const std::string & remoteGlobalIDString() const { return m_RemoteGlobalIDString; }
	inline void setRemoteGlobalIDString(const std::string & id) { m_RemoteGlobalIDString = id; }

	inline const std::string & remoteInfo() const { return m_RemoteInfo; }
	inline void setRemoteInfo(const std::string & info) { m_RemoteInfo = info; }

	std::string toString() const;

private:
	XCodeObject * m_ContainerPortal;
	std::string m_ProxyType;
	std::string m_RemoteGlobalIDString;
	std::string m_RemoteInfo;

	XCodeContainerItemProxy();
	~XCodeContainerItemProxy();

	friend class XCodeProject;
};

#endif
