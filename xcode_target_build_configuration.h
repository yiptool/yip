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
#ifndef __5cefef426b1c5f3cbadad67afb0ad9ae__
#define __5cefef426b1c5f3cbadad67afb0ad9ae__

#include "xcode_build_configuration.h"
#include <vector>
#include <string>

class XCodeProject;

class XCodeTargetBuildConfiguration : public XCodeBuildConfiguration
{
public:
	inline const std::string & assetCatalogAppIconName() const { return m_AssetCatalogAppIconName; }
	inline void setAssetCatalogAppIconName(const std::string & name) { m_AssetCatalogAppIconName = name; }

	inline const std::string & assetCatalogLaunchImageName() const { return m_AssetCatalogLaunchImageName; }
	inline void setAssetCatalogLaunchImageName(const std::string & name) { m_AssetCatalogLaunchImageName = name; }

	inline bool precompilePrefixHeader() const { return m_PrecompilePrefixHeader; }
	inline void setPrecompilePrefixHeader(bool flag) { m_PrecompilePrefixHeader = flag; }

	inline const std::string & prefixHeader() const { return m_PrefixHeader; }
	inline void setPrefixHeader(const std::string & hdr) { m_PrefixHeader = hdr; }

	inline const std::string & infoPListFile() const { return m_InfoPListFile; }
	inline void setInfoPListFile(const std::string & file) { m_InfoPListFile = file; }

	inline const std::string & productName() const { return m_ProductName; }
	inline void setProductName(const std::string & name) { m_ProductName = name; }

	inline const std::string & wrapperExtension() const { return m_WrapperExtension; }
	inline void setWrapperExtension(const std::string & ext) { m_WrapperExtension = ext; }

	inline void addPreprocessorDefinition(const std::string & def) { m_Defines.push_back(def); }

protected:
	void writeBuildSettings(std::stringstream & ss) const;

private:
	std::string m_AssetCatalogAppIconName;
	std::string m_AssetCatalogLaunchImageName;
	bool m_PrecompilePrefixHeader;
	std::string m_PrefixHeader;
	std::string m_InfoPListFile;
	std::string m_ProductName;
	std::string m_WrapperExtension;
	std::vector<std::string> m_Defines;

	XCodeTargetBuildConfiguration();
	~XCodeTargetBuildConfiguration();

	friend class XCodeProject;
};

#endif
