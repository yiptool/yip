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
#include "generate_xcode.h"
#include "../config.h"
#include "../xcode/xcode_project.h"
#include "../util/json_escape.h"
#include "../util/path.h"
#include "../util/file_type.h"
#include "../util/xml.h"
#include "../util/fmt.h"
#include <map>
#include <sstream>
#include <cassert>
#include <memory>

namespace
{
	struct Gen
	{
		// Input
		ProjectPtr project;
		bool iOS;

		// Output
		std::string projectPath;

		// Private
		std::string projectName;
		std::shared_ptr<XCodeProject> xcodeProject;
		bool somethingChanged = false;
		XCodeBuildPhase * frameworksBuildPhase = nullptr;
		XCodeBuildPhase * sourcesBuildPhase = nullptr;
		XCodeBuildPhase * resourcesBuildPhase = nullptr;
		XCodeGroup * mainGroup = nullptr;
		XCodeGroup * sourcesGroup = nullptr;
		XCodeGroup * frameworksGroup = nullptr;
		XCodeGroup * generatedGroup = nullptr;
		XCodeGroup * productsGroup = nullptr;
		XCodeGroup * resourcesGroup = nullptr;
		std::map<std::pair<XCodeGroup *, std::string>, XCodeGroup *> dirGroups;
		std::map<std::string, XCodeBuildPhase *> copyFilesBuildPhases;
		XCodeTargetBuildConfiguration * cfgTargetDebug = nullptr;
		XCodeTargetBuildConfiguration * cfgTargetRelease = nullptr;
		XCodeLegacyBuildConfiguration * cfgPreBuildDebug = nullptr;
		XCodeLegacyBuildConfiguration * cfgPreBuildRelease = nullptr;
		XCodeProjectBuildConfiguration * cfgProjectDebug = nullptr;
		XCodeProjectBuildConfiguration * cfgProjectRelease = nullptr;
		XCodeConfigurationList * targetCfgList = nullptr;
		XCodeConfigurationList * preBuildCfgList = nullptr;
		XCodeConfigurationList * projectCfgList = nullptr;
		XCodeLegacyTarget * preBuildTarget = nullptr;
		XCodeNativeTarget * nativeTarget = nullptr;

		/* Methods */

		// Build phases
		void createBuildPhases();
		XCodeBuildPhase * copyFilesPhaseForResourceDir(const std::string & path);

		// Groups
		void createGroups();
		XCodeGroup * groupForPath(XCodeGroup * rootGroup, const std::string & path);

		// Source files
		void addSourceFile(XCodeGroup * group, XCodeBuildPhase * phase, const SourceFilePtr & file);
		void addSourceFiles();

		// Resource files
		void addResourceFile(const SourceFilePtr & file);
		void addResourceFiles();

		// Configurations
		void initDebugConfiguration();
		void initReleaseConfiguration();
		void createConfigurationLists();

		// Preprocessor definitions
		void addDefines();

		// Targets
		void createPreBuildTarget();
		void createNativeTarget();

		// Frameworks
		void addFrameworks();

		// Auxiliary files
		void writeDummyResourceFile();
		void writeInfoPList();
		void writeImageAssets();
		void writeXCScheme();

		// Generating
		void writePBXProj();
		void generate();
	};
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private functions

static std::string fileTypeForXCode(FileType type)
{
	switch (type)
	{
	case FILE_UNKNOWN: return XCODE_FILETYPE_TEXT;				// FIXME
	case FILE_TEXT_PLAIN: return XCODE_FILETYPE_TEXT;
	case FILE_TEXT_HTML: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_TEXT_CSS: return XCODE_FILETYPE_TEXT;				// FIXME
	case FILE_TEXT_MARKDOWN: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_TEXT_XML: return XCODE_FILETYPE_TEXT;				// FIXME
	case FILE_ARCHIVE_ARJ: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_ARCHIVE_ZIP: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_ARCHIVE_RAR: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_ARCHIVE_TAR: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_ARCHIVE_TAR_GZ: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_ARCHIVE_TAR_BZ2: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_ARCHIVE_GZIP: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_ARCHIVE_BZIP2: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BUILD_MAKEFILE: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BUILD_CMAKE_SCRIPT: return XCODE_FILETYPE_TEXT;	// FIXME
	case FILE_BUILD_CMAKELISTS: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_WIN32_EXE: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_WIN32_DLL: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_WIN32_RES: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_WIN32_LIB: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_OBJECT: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_SHARED_OBJECT: return XCODE_FILETYPE_TEXT;	// FIXME
	case FILE_BINARY_ARCHIVE: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_BINARY_APPLE_FRAMEWORK: return XCODE_FILETYPE_WRAPPER_FRAMEWORK;
	case FILE_BINARY_APPLE_DYNAMIC_LIB: return XCODE_FILETYPE_COMPILED_MACHO_DYLIB;
	case FILE_SOURCE_C: return XCODE_FILETYPE_SOURCECODE_C_C;
	case FILE_SOURCE_CXX: return XCODE_FILETYPE_SOURCECODE_CPP_CPP;
	case FILE_SOURCE_C_HEADER: return XCODE_FILETYPE_SOURCECODE_C_H;
	case FILE_SOURCE_CXX_HEADER: return XCODE_FILETYPE_SOURCECODE_CPP_H;
	case FILE_SOURCE_OBJC: return XCODE_FILETYPE_SOURCECODE_C_OBJC;
	case FILE_SOURCE_OBJCXX: return XCODE_FILETYPE_SOURCECODE_CPP_OBJCPP;
	case FILE_SOURCE_JAVA: return XCODE_FILETYPE_SOURCECODE_JAVA;
	case FILE_SOURCE_JAVASCRIPT: return XCODE_FILETYPE_TEXT;	// FIXME
	case FILE_SOURCE_LUA: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_SOURCE_SHELL: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_SOURCE_PHP: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_SOURCE_PYTHON: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_SOURCE_WIN32_RC: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_SOURCE_WIN32_DEF: return XCODE_FILETYPE_TEXT;		// FIXME
	case FILE_SOURCE_PLIST: return XCODE_FILETYPE_TEXT_PLIST_XML;
	case FILE_IMAGE_XCASSETS: return XCODE_FILETYPE_FOLDER_ASSETCATALOG;
	case FILE_IMAGE_PNG: return XCODE_FILETYPE_IMAGE_PNG;
	case FILE_IMAGE_JPEG: return XCODE_FILETYPE_IMAGE_JPEG;
	case FILE_IMAGE_GIF: return XCODE_FILETYPE_TEXT;			// FIXME
	case FILE_IMAGE_BMP: return XCODE_FILETYPE_TEXT;			// FIXME
	}

	assert(false);
	throw std::runtime_error("internal error: invalid file type.");
}

static bool isCompilableFileType(FileType type)
{
	switch (type)
	{
	case FILE_SOURCE_C:
	case FILE_SOURCE_CXX:
	case FILE_SOURCE_OBJC:
	case FILE_SOURCE_OBJCXX:
		return true;
	default:
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Build phases

void Gen::createBuildPhases()
{
	frameworksBuildPhase = xcodeProject->addFrameworksBuildPhase();
	sourcesBuildPhase = xcodeProject->addSourcesBuildPhase();
	resourcesBuildPhase = xcodeProject->addResourcesBuildPhase();
}

XCodeBuildPhase * Gen::copyFilesPhaseForResourceDir(const std::string & path)
{
	if (path.length() == 0)
		return resourcesBuildPhase;

	auto it = copyFilesBuildPhases.find(path);
	if (it != copyFilesBuildPhases.end())
		return it->second;

	XCodeBuildPhase * phase = xcodeProject->addCopyFilesBuildPhase();
	phase->setDstPath(path);
	phase->setDstSubfolderSpec(XCodeBuildPhase::Subfolder_Resources);
	copyFilesBuildPhases.insert(std::make_pair(path, phase));

	return phase;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Groups

void Gen::createGroups()
{
	mainGroup = xcodeProject->addGroup();
	xcodeProject->setMainGroup(mainGroup);

	frameworksGroup = xcodeProject->addGroup();
	frameworksGroup->setName("Frameworks");
	mainGroup->addChild(frameworksGroup);

	generatedGroup = xcodeProject->addGroup();
	generatedGroup->setName("Generated");
	mainGroup->addChild(generatedGroup);

	productsGroup = xcodeProject->addGroup();
	productsGroup->setName("Products");
	xcodeProject->setProductRefGroup(productsGroup);
	mainGroup->addChild(productsGroup);

	resourcesGroup = xcodeProject->addGroup();
	resourcesGroup->setName("Resources");
	mainGroup->addChild(resourcesGroup);

	sourcesGroup = xcodeProject->addGroup();
	sourcesGroup->setName("Sources");
	mainGroup->addChild(sourcesGroup);
}

XCodeGroup * Gen::groupForPath(XCodeGroup * rootGroup, const std::string & path)
{
	std::pair<XCodeGroup *, std::string> key = std::make_pair(rootGroup, path);
	auto it = dirGroups.find(key);
	if (it != dirGroups.end())
		return it->second;

	std::string dir = pathGetDirectory(path);
	if (dir.length() > 0)
		rootGroup = groupForPath(rootGroup, dir);

	XCodeGroup * childGroup = xcodeProject->addGroup();
	childGroup->setName(pathGetFileName(path));
	childGroup->setSourceTree("<group>");
	rootGroup->addChild(childGroup);

	dirGroups.insert(std::make_pair(key, childGroup));

	return childGroup;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Source files

void Gen::addSourceFile(XCodeGroup * group, XCodeBuildPhase * phase, const SourceFilePtr & file)
{
	XCodeFileReference * ref = xcodeProject->addFileReference();
	ref->setPath(file->path());
	ref->setSourceTree("<absolute>");

	// Set file name
	std::string filename = pathGetFileName(file->name());
	if (filename != file->path())
		ref->setName(filename);

	// Set file type
	std::string explicitType = fileTypeForXCode(file->type());
	std::string lastKnownType = fileTypeForXCode(determineFileType(file->path()));
	if (lastKnownType == explicitType)
		ref->setLastKnownFileType(lastKnownType);
	else
		ref->setExplicitFileType(explicitType);

	// Add file into the group
	std::string path = pathGetDirectory(file->name());
	if (path.length() > 0)
		group = groupForPath(group, path);
	group->addChild(ref);

	// Add file to the build phase
	if (isCompilableFileType(file->type()))
	{
		XCodeBuildFile * buildFile = phase->addFile();
		if (file->isArcEnabled())
			buildFile->setCompilerFlags("-fobjc-arc");
		buildFile->setFileRef(ref);
	}
}

void Gen::addSourceFiles()
{
	for (auto it : project->sourceFiles())
	{
		const SourceFilePtr & file = it.second;
		if (!(file->platforms() & (iOS ? Platform::iOS : Platform::OSX)))
			continue;
		addSourceFile(file->isGenerated() ? generatedGroup : sourcesGroup, sourcesBuildPhase, file);
	}

	XCodeFileReference * ref = xcodeProject->addFileReference();
	ref->setPath(pathConcat(project->projectPath(), g_Config->projectFileName));
	ref->setSourceTree("<absolute>");
	ref->setName(g_Config->projectFileName);
	ref->setExplicitFileType(XCODE_FILETYPE_TEXT);
	mainGroup->addChild(ref);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Resource files

void Gen::addResourceFile(const SourceFilePtr & file)
{
	XCodeFileReference * ref = xcodeProject->addFileReference();
	ref->setPath(file->path());
	ref->setSourceTree("<absolute>");

	// Set file name
	std::string filename = pathGetFileName(file->name());
	if (filename != file->path())
		ref->setName(filename);

	// Set file type
	std::string explicitType = fileTypeForXCode(file->type());
	std::string lastKnownType = fileTypeForXCode(determineFileType(file->path()));
	if (lastKnownType == explicitType)
		ref->setLastKnownFileType(lastKnownType);
	else
		ref->setExplicitFileType(explicitType);

	// Add file into the group
	XCodeGroup * group = resourcesGroup;
	std::string path = pathGetDirectory(file->name());
	if (path.length() > 0)
		group = groupForPath(resourcesGroup, path);
	group->addChild(ref);

	// Add file to the build phase
	XCodeBuildPhase * buildPhase = copyFilesPhaseForResourceDir(path);
	XCodeBuildFile * buildFile = buildPhase->addFile();
	buildFile->setFileRef(ref);
}

void Gen::addResourceFiles()
{
	for (auto it : project->resourceFiles())
	{
		const SourceFilePtr & file = it.second;
		if (!(file->platforms() & (iOS ? Platform::iOS : Platform::OSX)))
			continue;
		addResourceFile(file);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Configurations

void Gen::initDebugConfiguration()
{
	cfgTargetDebug = xcodeProject->addTargetBuildConfiguration();
	cfgTargetDebug->setName("Debug");
	cfgTargetDebug->setInfoPListFile(projectName + "/Info.plist");
	cfgTargetDebug->setAssetCatalogAppIconName("AppIcon");
	if (!iOS)
		cfgTargetDebug->setCombineHiDpiImages(true);
	else
		cfgTargetDebug->setAssetCatalogLaunchImageName("LaunchImage");
	cfgTargetDebug->addHeaderSearchPath(pathConcat(project->yipDirectory()->path(), ".yip-import-proxies"));

	for (auto it : project->headerPaths())
	{
		const HeaderPathPtr & headerPath = it.second;
		if (!(headerPath->platforms() & (iOS ? Platform::iOS : Platform::OSX)))
			continue;
		cfgTargetDebug->addHeaderSearchPath(headerPath->path());
	}

	cfgPreBuildDebug = xcodeProject->addLegacyBuildConfiguration();
	cfgPreBuildDebug->setName("Debug");

	cfgProjectDebug = xcodeProject->addProjectBuildConfiguration();
	cfgProjectDebug->setName("Debug");
	if (!iOS)
		cfgProjectDebug->setArchs("$(ARCHS_STANDARD)");
	cfgProjectDebug->setGccOptimizationLevel("0");
	if (iOS)
		cfgProjectDebug->setIPhoneOSDeploymentTarget(project->iosDeploymentTarget());
	else
		cfgProjectDebug->setMacOSXDeploymentTarget(project->osxDeploymentTarget());
	cfgProjectDebug->setSDKRoot(iOS ? "iphoneos" : "macosx");

	std::stringstream family;
	const char * prefix = "";
	if (project->iosAllowIPhone())
		family << prefix << "1", prefix = ",";
	if (project->iosAllowIPad())
		family << prefix << "2", prefix = ",";
	cfgProjectDebug->setTargetedDeviceFamily(family.str());

	cfgProjectDebug->addPreprocessorDefinition("DEBUG=1");
	cfgProjectDebug->setCodeSignIdentity("iphoneos*", "iPhone Developer");	// FIXME: make configurable
}

void Gen::initReleaseConfiguration()
{
	cfgTargetRelease = xcodeProject->addTargetBuildConfiguration();
	cfgTargetRelease->setName("Release");
	cfgTargetRelease->setInfoPListFile(projectName + "/Info.plist");
	cfgTargetRelease->setAssetCatalogAppIconName("AppIcon");
	if (!iOS)
		cfgTargetRelease->setCombineHiDpiImages(true);
	else
		cfgTargetRelease->setAssetCatalogLaunchImageName("LaunchImage");
	cfgTargetRelease->addHeaderSearchPath(pathConcat(project->yipDirectory()->path(), ".yip-import-proxies"));

	for (auto it : project->headerPaths())
	{
		const HeaderPathPtr & headerPath = it.second;
		if (!(headerPath->platforms() & (iOS ? Platform::iOS : Platform::OSX)))
			continue;
		cfgTargetRelease->addHeaderSearchPath(headerPath->path());
	}

	cfgPreBuildRelease = xcodeProject->addLegacyBuildConfiguration();
	cfgPreBuildRelease->setName("Release");

	cfgProjectRelease = xcodeProject->addProjectBuildConfiguration();
	cfgProjectRelease->setName("Release");
	if (!iOS)
		cfgProjectRelease->setArchs("$(ARCHS_STANDARD)");
	cfgProjectRelease->setCopyPhaseStrip(true);
	cfgProjectRelease->setEnableNSAssertions(false);
	if (iOS)
		cfgProjectRelease->setIPhoneOSDeploymentTarget(project->iosDeploymentTarget());
	else
		cfgProjectRelease->setMacOSXDeploymentTarget(project->osxDeploymentTarget());
	cfgProjectRelease->setOnlyActiveArch(false);
	cfgProjectRelease->setSDKRoot(iOS ? "iphoneos" : "macosx");

	std::stringstream family;
	const char * prefix = "";
	if (project->iosAllowIPhone())
		family << prefix << "1", prefix = ",";
	if (project->iosAllowIPad())
		family << prefix << "2", prefix = ",";
	cfgProjectRelease->setTargetedDeviceFamily(family.str());

	cfgProjectRelease->setValidateProduct(true);
	cfgProjectRelease->addPreprocessorDefinition("NDEBUG=1");
	cfgProjectRelease->addPreprocessorDefinition("DISABLE_ASSERTIONS=1");
	cfgProjectRelease->setCodeSignIdentity("iphoneos*", "iPhone Developer");// FIXME: make configurable
}

void Gen::createConfigurationLists()
{
	targetCfgList = xcodeProject->addConfigurationList();
	targetCfgList->setDefaultConfigurationName("Release");
	targetCfgList->addConfiguration(cfgTargetDebug);
	targetCfgList->addConfiguration(cfgTargetRelease);

	preBuildCfgList = xcodeProject->addConfigurationList();
	preBuildCfgList->addConfiguration(cfgPreBuildDebug);
	preBuildCfgList->addConfiguration(cfgPreBuildRelease);

	projectCfgList = xcodeProject->addConfigurationList();
	projectCfgList->setDefaultConfigurationName("Release");
	projectCfgList->addConfiguration(cfgProjectDebug);
	projectCfgList->addConfiguration(cfgProjectRelease);
	xcodeProject->setBuildConfigurationList(projectCfgList);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Preprocessor definitions

void Gen::addDefines()
{
	for (auto it : project->defines())
	{
		const DefinePtr & define = it.second;
		if (!(define->platforms() & (iOS ? Platform::iOS : Platform::OSX)))
			continue;

		std::stringstream ss;
		for (char ch : define->name())
		{
			if (ch != '"')
				ss << ch;
			else
				ss << "\\\"";
		}
		std::string defineName = ss.str();

		if (define->buildTypes() & BuildType::Debug)
			cfgTargetDebug->addPreprocessorDefinition(defineName);
		if (define->buildTypes() & BuildType::Release)
			cfgTargetRelease->addPreprocessorDefinition(defineName);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Native target

void Gen::createPreBuildTarget()
{
	preBuildTarget = xcodeProject->addLegacyTarget();
	preBuildTarget->setName("yip-prebuild");
	preBuildTarget->setProductName("yip-prebuild");
	preBuildTarget->setBuildConfigurationList(preBuildCfgList);
	preBuildTarget->setBuildToolPath(pathGetThisExecutableFile());
	preBuildTarget->setBuildArgumentsString(fmt() << "xcode-prebuild " << (iOS ? "ios" : "osx") << " $(ACTION)");
	preBuildTarget->setBuildWorkingDirectory(project->projectPath());
	preBuildTarget->setPassBuildSettingsInEnvironment(true);
}

void Gen::createNativeTarget()
{
	XCodeFileReference * productRef = xcodeProject->addFileReference();
	productRef->setExplicitFileType(XCODE_FILETYPE_WRAPPER_APPLICATION);
	productRef->setIncludeInIndex(false);
	productRef->setPath(projectName + ".app");
	productRef->setSourceTree("BUILT_PRODUCTS_DIR");
	productsGroup->addChild(productRef);

	XCodeContainerItemProxy * preBuildProxy = xcodeProject->addContainerItemProxy();
	preBuildProxy->setContainerPortal(xcodeProject.get());
	preBuildProxy->setProxyType("1");
	preBuildProxy->setRemoteGlobalIDString(preBuildTarget->uniqueID().toString());
	preBuildProxy->setRemoteInfo(preBuildTarget->name());

	XCodeTargetDependency * preBuildDep = xcodeProject->addTargetDependency();
	preBuildDep->setTarget(preBuildTarget);
	preBuildDep->setTargetProxy(preBuildProxy);

	nativeTarget = xcodeProject->addNativeTarget();
	nativeTarget->setName(projectName);
	nativeTarget->setBuildConfigurationList(targetCfgList);
	nativeTarget->setProductName(projectName);
	nativeTarget->setProductReference(productRef);
	nativeTarget->addDependency(preBuildDep);
	nativeTarget->addBuildPhase(sourcesBuildPhase);
	nativeTarget->addBuildPhase(frameworksBuildPhase);
	nativeTarget->addBuildPhase(resourcesBuildPhase);
	for (auto it : copyFilesBuildPhases)
		nativeTarget->addBuildPhase(it.second);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Frameworks

void Gen::addFrameworks()
{
	std::set<std::string> libraryPaths;
	for (auto it : (iOS ? project->iosFrameworks() : project->osxFrameworks()))
	{
		std::string name = it.first;
		std::string path = it.second;
		bool sdkRoot = true;

		if (path.empty())
			path = pathConcat("System/Library/Frameworks", name);
		else if (path.length() >= 2 && path[0] == '$' && pathIsSeparator(path[1]))
			path = path.substr(2);
		else
			sdkRoot = false;

		XCodeFileReference * fileRef = xcodeProject->addFileReference();
		fileRef->setLastKnownFileType(fileTypeForXCode(determineFileType(name)));
		fileRef->setName(name);
		fileRef->setPath(path);
		fileRef->setSourceTree(sdkRoot ? "SDKROOT" : "<absolute>");
		frameworksGroup->addChild(fileRef);

		XCodeBuildFile * buildFile = frameworksBuildPhase->addFile();
		buildFile->setFileRef(fileRef);

		std::string ext = pathGetShortFileExtension(name);
		if (ext == ".framework" && !sdkRoot)
		{
			std::string searchPath = pathSimplify(pathGetDirectory(path));
			cfgTargetDebug->addFrameworkSearchPath(searchPath);
			cfgTargetRelease->addFrameworkSearchPath(searchPath);
		}
		else if (ext == ".dylib")
		{
			std::string dir = pathSimplify(pathGetDirectory(path));
			if (!dir.empty())
			{
				if (sdkRoot)
					dir = pathConcat("$(SDKROOT)", dir);
				if (dir != "$(SDKROOT)/usr/lib" && libraryPaths.insert(dir).second)
				{
					cfgProjectDebug->addLibrarySearchPath(dir);
					cfgProjectRelease->addLibrarySearchPath(dir);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Auxiliary files

void Gen::writeDummyResourceFile()
{
	XCodeFileReference * dummyFileRef = xcodeProject->addFileReference();
	dummyFileRef->setLastKnownFileType(XCODE_FILETYPE_TEXT);
	dummyFileRef->setName(".dummy");
	dummyFileRef->setPath(projectName + "/.dummy");
	dummyFileRef->setSourceTree("SOURCE_ROOT");
	resourcesGroup->addChild(dummyFileRef);

	XCodeBuildFile * buildFile = resourcesBuildPhase->addFile();
	buildFile->setFileRef(dummyFileRef);

	bool changed = false;
	project->yipDirectory()->writeFile(projectName + "/.dummy", std::string(), &changed);
	somethingChanged = somethingChanged || changed;
}

void Gen::writeInfoPList()
{
	XCodeFileReference * plistFileRef = xcodeProject->addFileReference();
	plistFileRef->setLastKnownFileType(XCODE_FILETYPE_TEXT_PLIST_XML);
	plistFileRef->setName("Info.plist");
	plistFileRef->setPath(projectName + "/Info.plist");
	plistFileRef->setSourceTree("SOURCE_ROOT");

	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" "
		"\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
	ss << "<plist version=\"1.0\">\n";
	ss << "<dict>\n";
	ss << "\t<key>CFBundleDevelopmentRegion</key>\n";
	ss << "\t<string>en</string>\n";								// FIXME: make configurable
	if (iOS)
	{
		ss << "\t<key>CFBundleDisplayName</key>\n";
		ss << "\t<string>" << xmlEscape(project->iosBundleDisplayName()) << "</string>\n";
	}
	ss << "\t<key>CFBundleExecutable</key>\n";
	ss << "\t<string>${EXECUTABLE_NAME}</string>\n";				// FIXME: make configurable
	if (!iOS)
	{
		ss << "\t<key>CFBundleIconFile</key>\n";
		ss << "\t<string></string>\n";
	}
	ss << "\t<key>CFBundleIdentifier</key>\n";
	if (iOS)
		ss << "\t<string>" << xmlEscape(project->iosBundleIdentifier()) << "</string>\n";
	else
		ss << "\t<string>" << xmlEscape(project->osxBundleIdentifier()) << "</string>\n";
	ss << "\t<key>CFBundleInfoDictionaryVersion</key>\n";
	ss << "\t<string>6.0</string>\n";
	ss << "\t<key>CFBundleName</key>\n";
	ss << "\t<string>${PRODUCT_NAME}</string>\n";					// FIXME: make configurable
	ss << "\t<key>CFBundlePackageType</key>\n";
	ss << "\t<string>APPL</string>\n";
	ss << "\t<key>CFBundleShortVersionString</key>\n";
	if (iOS)
		ss << "\t<string>" << xmlEscape(project->iosBundleVersion()) << "</string>\n";
	else
		ss << "\t<string>" << xmlEscape(project->osxBundleVersion()) << "</string>\n";
	ss << "\t<key>CFBundleSignature</key>\n";
	ss << "\t<string>\?\?\?\?</string>\n";
	if (iOS && (!project->iosFacebookAppID().empty() || !project->iosVkAppID().empty()))
	{
		ss << "\t<key>CFBundleURLTypes</key>\n";
		ss << "\t<array>\n";
		if (!project->iosFacebookAppID().empty())
		{
			ss << "\t\t<dict>\n";
			ss << "\t\t\t<key>CFBundleTypeRole</key>\n";
			ss << "\t\t\t<string>Editor</string>\n";
			ss << "\t\t\t<key>CFBundleURLIconFile</key>\n";
			ss << "\t\t\t<string>Default</string>\n";
			ss << "\t\t\t<key>CFBundleURLSchemes</key>\n";
			ss << "\t\t\t<array>\n";
			ss << "\t\t\t\t<string>fb" << xmlEscape(project->iosFacebookAppID()) << "</string>\n";
			ss << "\t\t\t</array>\n";
			ss << "\t\t</dict>\n";
		}
		if (!project->iosVkAppID().empty())
		{
			ss << "\t\t<dict>\n";
			ss << "\t\t\t<key>CFBundleTypeRole</key>\n";
			ss << "\t\t\t<string>Editor</string>\n";
			ss << "\t\t\t<key>CFBundleURLIconFile</key>\n";
			ss << "\t\t\t<string>Default</string>\n";
			ss << "\t\t\t<key>CFBundleURLSchemes</key>\n";
			ss << "\t\t\t<array>\n";
			ss << "\t\t\t\t<string>vk" << xmlEscape(project->iosVkAppID()) << "</string>\n";
			ss << "\t\t\t</array>\n";
			ss << "\t\t</dict>\n";
		}
		ss << "\t</array>\n";
	}
	ss << "\t<key>CFBundleVersion</key>\n";
	if (iOS)
		ss << "\t<string>" << xmlEscape(project->iosBundleVersion()) << "</string>\n";
	else
		ss << "\t<string>" << xmlEscape(project->osxBundleVersion()) << "</string>\n";
	if (!iOS)
	{
		ss << "\t<key>LSApplicationCategoryType</key>\n";
		ss << "\t<string>public.app-category.games</string>\n";		// FIXME: make configurable
		ss << "\t<key>LSMinimumSystemVersion</key>\n";
		ss << "\t<string>${MACOSX_DEPLOYMENT_TARGET}</string>\n";	// FIXME: make configurable
		ss << "\t<key>NSHumanReadableCopyright</key>\n";
		ss << "\t<string>Copyright © 2014. All rights reserved.</string>\n";	// FIXME: make configurable
	}
	else
	{
		if (!project->iosFacebookAppID().empty())
		{
			ss << "\t<key>FacebookAppID</key>\n";
			ss << "\t<string>" << xmlEscape(project->iosFacebookAppID()) << "</string>\n";
		}
		if (!project->iosFacebookDisplayName().empty())
		{
			ss << "\t<key>FacebookDisplayName</key>\n";
			ss << "\t<string>" << xmlEscape(project->iosFacebookDisplayName()) << "</string>\n";
		}
		ss << "\t<key>LSRequiresIPhoneOS</key>\n";
		ss << "\t<true/>\n";
		if (!project->iosFonts().empty())
		{
			ss << "<key>UIAppFonts</key>\n";
			ss << "<array>\n";
			for (const std::string & font : project->iosFonts())
				ss << "\t<string>" << xmlEscape(font) << "</string>\n";
			ss << "</array>\n";
		}
		ss << "\t<key>UIRequiredDeviceCapabilities</key>\n";
		ss << "\t<array>\n";
		ss << "\t\t<string>armv7</string>\n";							// FIXME: make configurable
		ss << "\t</array>\n";
		ss << "\t<key>UISupportedInterfaceOrientations</key>\n";		// FIXME: make configurable
		ss << "\t<array>\n";
		ss << "\t\t<string>UIInterfaceOrientationPortrait</string>\n";
		ss << "\t\t<string>UIInterfaceOrientationLandscapeLeft</string>\n";
		ss << "\t\t<string>UIInterfaceOrientationLandscapeRight</string>\n";
		ss << "\t</array>\n";
		ss << "\t<key>UISupportedInterfaceOrientations~ipad</key>\n";	// FIXME: make configurable
		ss << "\t<array>\n";
		ss << "\t\t<string>UIInterfaceOrientationPortrait</string>\n";
		ss << "\t\t<string>UIInterfaceOrientationPortraitUpsideDown</string>\n";
		ss << "\t\t<string>UIInterfaceOrientationLandscapeLeft</string>\n";
		ss << "\t\t<string>UIInterfaceOrientationLandscapeRight</string>\n";
		ss << "\t</array>\n";
	}
	ss << "</dict>\n";
	ss << "</plist>\n";

	bool changed = false;
	project->yipDirectory()->writeFile(projectName + "/Info.plist", ss.str(), &changed);
	somethingChanged = somethingChanged || changed;
}

void Gen::writeImageAssets()
{
	XCodeFileReference * assetsFileRef = xcodeProject->addFileReference();
	assetsFileRef->setLastKnownFileType(XCODE_FILETYPE_FOLDER_ASSETCATALOG);
	assetsFileRef->setName("Images.xcassets");
	assetsFileRef->setPath(projectName + "/Images.xcassets");
	assetsFileRef->setSourceTree("SOURCE_ROOT");
	resourcesGroup->addChild(assetsFileRef);

	XCodeBuildFile * buildFile = resourcesBuildPhase->addFile();
	buildFile->setFileRef(assetsFileRef);

	std::string assetsDir = pathConcat(
		project->yipDirectory()->path(),
		projectName + "/Images.xcassets/AppIcon.appiconset"
	);

	std::stringstream ss;
	ss << "{\n";
	ss << "  \"images\" : [\n";
	if (iOS)
	{
		const std::map<Project::ImageSize, std::string> & icons = project->iosIcons();
		std::map<Project::ImageSize, std::string>::const_iterator it;

		#define IOS_ICON(SIZE) \
			if ((it = icons.find(SIZE)) != icons.end()) \
			{ \
				std::string name = fmt() << SIZE << ".png"; \
				std::string path = pathConcat(assetsDir, name); \
				pathCreate(pathGetDirectory(path)); \
				pathCreateSymLink(it->second, path); \
				ss << "      \"filename\" : \"" << jsonEscape(name) << "\",\n"; \
			}

		ss << "    {\n";
		ss << "      \"idiom\" : \"iphone\",\n";
		ss << "      \"size\" : \"57x57\",\n";
		IOS_ICON(Project::IMAGESIZE_IPHONE_STANDARD)
		ss << "      \"scale\" : \"1x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"size\" : \"57x57\",\n";
		ss << "      \"idiom\" : \"iphone\",\n";
		IOS_ICON(Project::IMAGESIZE_IPHONE_RETINA_IOS6)
		ss << "      \"scale\" : \"2x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"size\" : \"60x60\",\n";
		ss << "      \"idiom\" : \"iphone\",\n";
		IOS_ICON(Project::IMAGESIZE_IPHONE_RETINA)
		ss << "      \"scale\" : \"2x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"idiom\" : \"ipad\",\n";
		ss << "      \"size\" : \"72x72\",\n";
		IOS_ICON(Project::IMAGESIZE_IPAD_STANDARD_IOS6)
		ss << "      \"scale\" : \"1x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"size\" : \"72x72\",\n";
		ss << "      \"idiom\" : \"ipad\",\n";
		IOS_ICON(Project::IMAGESIZE_IPAD_RETINA_IOS6)
		ss << "      \"scale\" : \"2x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"size\" : \"76x76\",\n";
		ss << "      \"idiom\" : \"ipad\",\n";
		IOS_ICON(Project::IMAGESIZE_IPAD_STANDARD)
		ss << "      \"scale\" : \"1x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"size\" : \"76x76\",\n";
		ss << "      \"idiom\" : \"ipad\",\n";
		IOS_ICON(Project::IMAGESIZE_IPAD_RETINA)
		ss << "      \"scale\" : \"2x\"\n";
		ss << "    }\n";
	}
	else
	{
		// FIXME
		ss << "    {\n";
		ss << "      \"idiom\" : \"mac\",\n";
		ss << "      \"size\" : \"16x16\",\n";
		ss << "      \"scale\" : \"1x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"idiom\" : \"mac\",\n";
		ss << "      \"size\" : \"16x16\",\n";
		ss << "      \"scale\" : \"2x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"idiom\" : \"mac\",\n";
		ss << "      \"size\" : \"32x32\",\n";
		ss << "      \"scale\" : \"1x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"idiom\" : \"mac\",\n";
		ss << "      \"size\" : \"32x32\",\n";
		ss << "      \"scale\" : \"2x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"idiom\" : \"mac\",\n";
		ss << "      \"size\" : \"128x128\",\n";
		ss << "      \"scale\" : \"1x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"idiom\" : \"mac\",\n";
		ss << "      \"size\" : \"128x128\",\n";
		ss << "      \"scale\" : \"2x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"idiom\" : \"mac\",\n";
		ss << "      \"size\" : \"256x256\",\n";
		ss << "      \"scale\" : \"1x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"idiom\" : \"mac\",\n";
		ss << "      \"size\" : \"256x256\",\n";
		ss << "      \"scale\" : \"2x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"idiom\" : \"mac\",\n";
		ss << "      \"size\" : \"512x512\",\n";
		ss << "      \"scale\" : \"1x\"\n";
		ss << "    },\n";
		ss << "    {\n";
		ss << "      \"idiom\" : \"mac\",\n";
		ss << "      \"size\" : \"512x512\",\n";
		ss << "      \"scale\" : \"2x\"\n";
		ss << "    }\n";
	}
	ss << "  ],\n";
	ss << "  \"info\" : {\n";
	ss << "    \"version\" : 1,\n";
	ss << "    \"author\" : \"xcode\"\n";
	ss << "  }\n";
	ss << "}\n";

	bool changed = false;
	project->yipDirectory()
		->writeFile(projectName + "/Images.xcassets/AppIcon.appiconset/Contents.json", ss.str(), &changed);
	somethingChanged = somethingChanged || changed;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	assetsDir = pathConcat(
		project->yipDirectory()->path(),
		projectName + "/Images.xcassets/LaunchImage.launchimage"
	);

	if (iOS)
	{
		const std::map<Project::ImageSize, std::string> & launchImages = project->iosLaunchImages();
		std::map<Project::ImageSize, std::string>::const_iterator it;

		#define IOS_LAUNCHIMAGE(SIZE) \
			if ((it = launchImages.find(SIZE)) != launchImages.end()) \
			{ \
				std::string name = fmt() << SIZE << ".png"; \
				std::string path = pathConcat(assetsDir, name); \
				pathCreate(pathGetDirectory(path)); \
				pathCreateSymLink(it->second, path); \
				ss2 << "      \"filename\" : \"" << jsonEscape(name) << "\",\n"; \
			}

		std::stringstream ss2;
		ss2 << "{\n";
		ss2 << "  \"images\" : [\n";
		ss2 << "    {\n";
		ss2 << "      \"orientation\" : \"portrait\",\n";
		ss2 << "      \"idiom\" : \"iphone\",\n";
		ss2 << "      \"extent\" : \"full-screen\",\n";
		IOS_LAUNCHIMAGE(Project::IMAGESIZE_LAUNCH_IPHONE_STANDARD)
		ss2 << "      \"scale\" : \"1x\"\n";
		ss2 << "    },\n";
		ss2 << "    {\n";
		ss2 << "      \"orientation\" : \"portrait\",\n";
		ss2 << "      \"idiom\" : \"iphone\",\n";
		ss2 << "      \"extent\" : \"full-screen\",\n";
		ss2 << "      \"minimum-system-version\" : \"7.0\",\n";
		IOS_LAUNCHIMAGE(Project::IMAGESIZE_LAUNCH_IPHONE_RETINA)
		ss2 << "      \"scale\" : \"2x\"\n";
		ss2 << "    },\n";
		ss2 << "    {\n";
		ss2 << "      \"orientation\" : \"portrait\",\n";
		ss2 << "      \"idiom\" : \"iphone\",\n";
		ss2 << "      \"extent\" : \"full-screen\",\n";
		IOS_LAUNCHIMAGE(Project::IMAGESIZE_LAUNCH_IPHONE_RETINA)
		ss2 << "      \"scale\" : \"2x\"\n";
		ss2 << "    },\n";
		ss2 << "    {\n";
		ss2 << "      \"extent\" : \"full-screen\",\n";
		ss2 << "      \"idiom\" : \"iphone\",\n";
		ss2 << "      \"subtype\" : \"retina4\",\n";
		IOS_LAUNCHIMAGE(Project::IMAGESIZE_LAUNCH_IPHONE5_RETINA)
		ss2 << "      \"minimum-system-version\" : \"7.0\",\n";
		ss2 << "      \"orientation\" : \"portrait\",\n";
		ss2 << "      \"scale\" : \"2x\"\n";
		ss2 << "    },\n";
		ss2 << "    {\n";
		ss2 << "      \"orientation\" : \"portrait\",\n";
		ss2 << "      \"idiom\" : \"iphone\",\n";
		ss2 << "      \"extent\" : \"full-screen\",\n";
		IOS_LAUNCHIMAGE(Project::IMAGESIZE_LAUNCH_IPHONE5_RETINA)
		ss2 << "      \"subtype\" : \"retina4\",\n";
		ss2 << "      \"scale\" : \"2x\"\n";
		ss2 << "    },\n";
		ss2 << "    {\n";
		ss2 << "      \"orientation\" : \"portrait\",\n";
		ss2 << "      \"idiom\" : \"ipad\",\n";
		ss2 << "      \"extent\" : \"full-screen\",\n";
		ss2 << "      \"minimum-system-version\" : \"7.0\",\n";
		IOS_LAUNCHIMAGE(Project::IMAGESIZE_LAUNCH_IPAD_PORTRAIT)
		ss2 << "      \"scale\" : \"1x\"\n";
		ss2 << "    },\n";
		ss2 << "    {\n";
		ss2 << "      \"orientation\" : \"portrait\",\n";
		ss2 << "      \"idiom\" : \"ipad\",\n";
		ss2 << "      \"extent\" : \"full-screen\",\n";
		IOS_LAUNCHIMAGE(Project::IMAGESIZE_LAUNCH_IPAD_PORTRAIT)
		ss2 << "      \"scale\" : \"1x\"\n";
		ss2 << "    },\n";
		ss2 << "    {\n";
		ss2 << "      \"orientation\" : \"portrait\",\n";
		ss2 << "      \"idiom\" : \"ipad\",\n";
		ss2 << "      \"extent\" : \"full-screen\",\n";
		ss2 << "      \"minimum-system-version\" : \"7.0\",\n";
		IOS_LAUNCHIMAGE(Project::IMAGESIZE_LAUNCH_IPAD_PORTRAIT_RETINA)
		ss2 << "      \"scale\" : \"2x\"\n";
		ss2 << "    },\n";
		ss2 << "    {\n";
		ss2 << "      \"orientation\" : \"portrait\",\n";
		ss2 << "      \"idiom\" : \"ipad\",\n";
		ss2 << "      \"extent\" : \"full-screen\",\n";
		IOS_LAUNCHIMAGE(Project::IMAGESIZE_LAUNCH_IPAD_PORTRAIT_RETINA)
		ss2 << "      \"scale\" : \"2x\"\n";
		ss2 << "    }\n";
		ss2 << "  ],\n";
		ss2 << "  \"info\" : {\n";
		ss2 << "    \"version\" : 1,\n";
		ss2 << "    \"author\" : \"xcode\"\n";
		ss2 << "  }\n";
		ss2 << "}\n";

		bool chgd = false;
		project->yipDirectory()
			->writeFile(projectName + "/Images.xcassets/LaunchImage.launchimage/Contents.json", ss2.str(), &chgd);
		somethingChanged = somethingChanged || chgd;
	}
}

void Gen::writeXCScheme()
{
	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<Scheme\n";
	ss << "   LastUpgradeVersion = \"0510\"\n";
	ss << "   version = \"1.3\">\n";
	ss << "   <BuildAction\n";
	ss << "      parallelizeBuildables = \"YES\"\n";
	ss << "      buildImplicitDependencies = \"YES\">\n";
	ss << "      <BuildActionEntries>\n";
	ss << "         <BuildActionEntry\n";
	ss << "            buildForTesting = \"YES\"\n";
	ss << "            buildForRunning = \"YES\"\n";
	ss << "            buildForProfiling = \"YES\"\n";
	ss << "            buildForArchiving = \"YES\"\n";
	ss << "            buildForAnalyzing = \"YES\">\n";
	ss << "            <BuildableReference\n";
	ss << "               BuildableIdentifier = \"primary\"\n";
	ss << "               BlueprintIdentifier = \"" << nativeTarget->uniqueID().toString() << "\"\n";
	ss << "               BuildableName = \"" << projectName << ".app\"\n";
	ss << "               BlueprintName = \"" << projectName << "\"\n";
	ss << "               ReferencedContainer = \"container:" << projectName << ".xcodeproj\">\n";
	ss << "            </BuildableReference>\n";
	ss << "         </BuildActionEntry>\n";
	ss << "      </BuildActionEntries>\n";
	ss << "   </BuildAction>\n";
	ss << "   <TestAction\n";
	ss << "      selectedDebuggerIdentifier = \"Xcode.DebuggerFoundation.Debugger.LLDB\"\n";
	ss << "      selectedLauncherIdentifier = \"Xcode.DebuggerFoundation.Launcher.LLDB\"\n";
	ss << "      shouldUseLaunchSchemeArgsEnv = \"YES\"\n";
	ss << "      buildConfiguration = \"Debug\">\n";
	ss << "      <Testables>\n";
	ss << "      </Testables>\n";
	ss << "      <MacroExpansion>\n";
	ss << "         <BuildableReference\n";
	ss << "            BuildableIdentifier = \"primary\"\n";
	ss << "            BlueprintIdentifier = \"" << nativeTarget->uniqueID().toString() << "\"\n";
	ss << "            BuildableName = \"" << projectName << ".app\"\n";
	ss << "            BlueprintName = \"" << projectName << "\"\n";
	ss << "            ReferencedContainer = \"container:" << projectName << ".xcodeproj\">\n";
	ss << "         </BuildableReference>\n";
	ss << "      </MacroExpansion>\n";
	ss << "   </TestAction>\n";
	ss << "   <LaunchAction\n";
	ss << "      selectedDebuggerIdentifier = \"Xcode.DebuggerFoundation.Debugger.LLDB\"\n";
	ss << "      selectedLauncherIdentifier = \"Xcode.DebuggerFoundation.Launcher.LLDB\"\n";
	ss << "      launchStyle = \"0\"\n";
	ss << "      useCustomWorkingDirectory = \"NO\"\n";
	ss << "      buildConfiguration = \"Debug\"\n";
	ss << "      ignoresPersistentStateOnLaunch = \"NO\"\n";
	ss << "      debugDocumentVersioning = \"YES\"\n";
	ss << "      allowLocationSimulation = \"YES\">\n";
	ss << "      <BuildableProductRunnable>\n";
	ss << "         <BuildableReference\n";
	ss << "            BuildableIdentifier = \"primary\"\n";
	ss << "            BlueprintIdentifier = \"" << nativeTarget->uniqueID().toString() << "\"\n";
	ss << "            BuildableName = \"" << projectName << ".app\"\n";
	ss << "            BlueprintName = \"" << projectName << "\"\n";
	ss << "            ReferencedContainer = \"container:" << projectName << ".xcodeproj\">\n";
	ss << "         </BuildableReference>\n";
	ss << "      </BuildableProductRunnable>\n";
	ss << "      <AdditionalOptions>\n";
	ss << "      </AdditionalOptions>\n";
	ss << "   </LaunchAction>\n";
	ss << "   <ProfileAction\n";
	ss << "      shouldUseLaunchSchemeArgsEnv = \"YES\"\n";
	ss << "      savedToolIdentifier = \"\"\n";
	ss << "      useCustomWorkingDirectory = \"NO\"\n";
	ss << "      buildConfiguration = \"Release\"\n";
	ss << "      debugDocumentVersioning = \"YES\">\n";
	ss << "      <BuildableProductRunnable>\n";
	ss << "         <BuildableReference\n";
	ss << "            BuildableIdentifier = \"primary\"\n";
	ss << "            BlueprintIdentifier = \"" << nativeTarget->uniqueID().toString() << "\"\n";
	ss << "            BuildableName = \"" << projectName << ".app\"\n";
	ss << "            BlueprintName = \"" << projectName << "\"\n";
	ss << "            ReferencedContainer = \"container:" << projectName << ".xcodeproj\">\n";
	ss << "         </BuildableReference>\n";
	ss << "      </BuildableProductRunnable>\n";
	ss << "   </ProfileAction>\n";
	ss << "   <AnalyzeAction\n";
	ss << "      buildConfiguration = \"Debug\">\n";
	ss << "   </AnalyzeAction>\n";
	ss << "   <ArchiveAction\n";
	ss << "      buildConfiguration = \"Release\"\n";
	ss << "      revealArchiveInOrganizer = \"YES\">\n";
	ss << "   </ArchiveAction>\n";
	ss << "</Scheme>\n";

	bool chgd = false;
	project->yipDirectory()
		->writeFile(projectName + ".xcodeproj/xcshareddata/xcschemes/" + projectName + ".xcscheme", ss.str(), &chgd);
	somethingChanged = somethingChanged || chgd;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generating

void Gen::writePBXProj()
{
	bool chd = false;
	project->yipDirectory()->writeFile(projectName + ".xcodeproj/project.pbxproj", xcodeProject->toString(), &chd);
	somethingChanged = somethingChanged || chd;
}

void Gen::generate()
{
	projectName = (iOS ? "ios" : "osx");
	projectPath = pathConcat(project->yipDirectory()->path(), projectName) + ".xcodeproj";

	xcodeProject = std::make_shared<XCodeProject>();
	xcodeProject->setOrganizationName("");									// FIXME: make configurable

	createBuildPhases();
	createGroups();
	addSourceFiles();
	addResourceFiles();
	initDebugConfiguration();
	initReleaseConfiguration();
	addDefines();
	createConfigurationLists();
	createPreBuildTarget();
	createNativeTarget();
	addFrameworks();

	writeDummyResourceFile();
	writeInfoPList();
	writeImageAssets();
	writeXCScheme();

	writePBXProj();
}

std::string generateXCode(const ProjectPtr & project, bool iOS, bool * changed)
{
	Gen gen;
	gen.project = project;
	gen.iOS = iOS;
	gen.generate();
	if (changed)
		*changed = gen.somethingChanged;
	return gen.projectPath;
}
