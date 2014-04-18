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
#ifndef __b452a1405c1a5aa54336e360b8b4fe1f__
#define __b452a1405c1a5aa54336e360b8b4fe1f__

#include "source_file.h"
#include "header_path.h"
#include "define.h"
#include "yip_directory.h"
#include "../util/git.h"
#include <vector>
#include <map>
#include <set>
#include <memory>

class Project : public std::enable_shared_from_this<Project>
{
public:
	enum ImageSize
	{
		IMAGESIZE_INVALID = 0,
		// iOS icons
		IMAGESIZE_IPHONE_STANDARD,				// 57x57
		IMAGESIZE_IPAD_STANDARD_IOS6,			// 72x72
		IMAGESIZE_IPAD_STANDARD,				// 76x76
		IMAGESIZE_IPHONE_RETINA_IOS6,			// 114x114
		IMAGESIZE_IPHONE_RETINA,				// 120x120
		IMAGESIZE_IPAD_RETINA_IOS6,				// 144x144
		IMAGESIZE_IPAD_RETINA,					// 152x152
		// iOS launch images
		IMAGESIZE_LAUNCH_IPHONE_STANDARD,		// 320x480
		IMAGESIZE_LAUNCH_IPHONE_RETINA,			// 640x960
		IMAGESIZE_LAUNCH_IPHONE5_RETINA,		// 640x1136
		IMAGESIZE_LAUNCH_IPAD_PORTRAIT,			// 768x1024
		IMAGESIZE_LAUNCH_IPAD_PORTRAIT_RETINA,	// 1536x2048
	};

	Project(const std::string & prjPath);
	~Project();

	inline bool isValid() const { return m_Valid; }
	inline void setValid(bool flag) { m_Valid = flag; }

	inline const std::string & projectPath() const { return m_ProjectPath; }
	const YipDirectoryPtr & yipDirectory() const;

	SourceFilePtr addSourceFile(const std::string & name, const std::string & path);
	inline const std::map<std::string, SourceFilePtr> & sourceFiles() const { return m_SourceFiles; }

	SourceFilePtr addResourceFile(const std::string & name, const std::string & path);
	inline const std::map<std::string, SourceFilePtr> & resourceFiles() const { return m_ResourceFiles; }

	DefinePtr addDefine(const std::string & name, Platform::Type platforms = Platform::All,
		BuildType::Value buildTypes = BuildType::All);
	inline const std::map<std::string, DefinePtr> & defines() const { return m_Defines; }

	inline bool addImport(const std::string & name, const std::string & url)
		{ return m_Imports.insert(std::make_pair(name, url)).second; }
	inline const std::map<std::string, std::string> & imports() const { return m_Imports; }

	void addHeaderPath(const std::string & path, Platform::Type platform);
	inline const std::map<std::string, HeaderPathPtr> & headerPaths() const { return m_HeaderPaths; }

	// WinRT

	void winrtAddLibrary(const std::string & name);
	inline const std::set<std::string> & winrtLibraries() const { return m_WinRTLibraries; }

	// OSX

	void osxSetBundleIdentifier(const std::string & val) { m_OSXBundleIdentifier = val; }
	inline const std::string & osxBundleIdentifier() const { return m_OSXBundleIdentifier; }

	void osxSetBundleVersion(const std::string & val) { m_OSXBundleVersion = val; }
	inline const std::string & osxBundleVersion() const { return m_OSXBundleVersion; }

	void osxAddFramework(const std::string & name, const std::string & path);
	inline const std::map<std::string, std::string> & osxFrameworks() const { return m_OSXFrameworks; }

	void osxAddIcon(ImageSize size, const std::string & path);
	inline const std::map<ImageSize, std::string> & osxIcons() const { return m_OSXIcons; }

	// iOS

	void iosSetBundleIdentifier(const std::string & val) { m_IOSBundleIdentifier = val; }
	inline const std::string & iosBundleIdentifier() const { return m_IOSBundleIdentifier; }

	void iosSetBundleVersion(const std::string & val) { m_IOSBundleVersion = val; }
	inline const std::string & iosBundleVersion() const { return m_IOSBundleVersion; }

	void iosSetBundleDisplayName(const std::string & val) { m_IOSBundleDisplayName = val; }
	inline const std::string & iosBundleDisplayName() const { return m_IOSBundleDisplayName; }

	void iosAddFramework(const std::string & name, const std::string & path);
	inline const std::map<std::string, std::string> & iosFrameworks() const { return m_IOSFrameworks; }

	void iosAddIcon(ImageSize size, const std::string & path);
	inline const std::map<ImageSize, std::string> & iosIcons() const { return m_IOSIcons; }

	void iosAddLaunchImage(ImageSize size, const std::string & path);
	inline const std::map<ImageSize, std::string> & iosLaunchImages() const { return m_IOSLaunchImages; }

	inline void iosAddFont(const std::string & font) { m_IOSFonts.insert(font); }
	inline const std::set<std::string> & iosFonts() const { return m_IOSFonts; }

	inline void iosSetAllowIPad(bool flag) { m_IOSAllowIPad = flag; }
	inline void iosSetAllowIPhone(bool flag) { m_IOSAllowIPhone = flag; }
	inline bool iosAllowIPad() const { return m_IOSAllowIPad; }
	inline bool iosAllowIPhone() const { return m_IOSAllowIPhone; }

	// Tizen

	void tizenAddPrivilege(const std::string & url);
	inline const std::set<std::string> & tizenPrivileges() const { return m_TizenPrivileges; }

	// Licenses

	inline void addLicense(const std::string & license) { m_Licenses.push_back(license); }

	// Code generation

	void generateLicenseData();

private:
	std::string m_ProjectPath;
	std::map<std::string, HeaderPathPtr> m_HeaderPaths;
	std::map<std::string, SourceFilePtr> m_SourceFiles;
	std::map<std::string, SourceFilePtr> m_ResourceFiles;
	std::map<std::string, DefinePtr> m_Defines;
	std::map<std::string, std::string> m_Imports;
	std::map<std::string, std::string> m_OSXFrameworks;
	std::map<std::string, std::string> m_IOSFrameworks;
	std::map<ImageSize, std::string> m_OSXIcons;
	std::map<ImageSize, std::string> m_IOSIcons;
	std::map<ImageSize, std::string> m_IOSLaunchImages;
	std::set<std::string> m_IOSFonts;
	std::set<std::string> m_WinRTLibraries;
	std::set<std::string> m_TizenPrivileges;
	std::vector<std::string> m_Licenses;
	std::string m_OSXBundleIdentifier;
	std::string m_OSXBundleVersion;
	std::string m_IOSBundleIdentifier;
	std::string m_IOSBundleVersion;
	std::string m_IOSBundleDisplayName;
	mutable YipDirectoryPtr m_YipDirectory;
	bool m_IOSAllowIPad;
	bool m_IOSAllowIPhone;
	bool m_Valid;

	void addFramework(std::map<std::string, std::string> & map, const std::string & name,
		const std::string & path, const char * what);

	Project(const Project &) = delete;
	Project & operator=(const Project &) = delete;
};

typedef std::shared_ptr<Project> ProjectPtr;

#endif
