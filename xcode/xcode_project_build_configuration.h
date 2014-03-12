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
#ifndef __c7386a7c1f865726067bebd964f4a475__
#define __c7386a7c1f865726067bebd964f4a475__

#include "xcode_build_configuration.h"
#include <vector>
#include <string>

class XCodeProject;

enum XCodeWarnFlag
{
	XCodeDisableWarning = 0,
	XCodeEnableWarning,
	XCodeWarningAsError
};

class XCodeProjectBuildConfiguration : public XCodeBuildConfiguration
{
public:
	inline bool alwaysSearchUserPaths() const { return m_AlwaysSearchUserPaths; }
	inline void setAlwaysSearchUserPaths(bool flag) { m_AlwaysSearchUserPaths = flag; }

	inline const std::string & archs() const { return m_Archs; }
	inline void setArchs(const std::string & archs_) { m_Archs = archs_; }

	inline const std::string & clangCxxLanguageStandard() const { return m_ClangCxxLanguageStandard; }
	inline void setClangCxxLanguageStandard(const std::string & std) { m_ClangCxxLanguageStandard = std; }

	inline const std::string & clangCxxLibrary() const { return m_ClangCxxLibrary; }
	inline void setClangCxxLibrary(const std::string & std) { m_ClangCxxLibrary = std; }

	inline bool clangEnableModules() const { return m_ClangEnableModules; }
	inline void setClangEnableModules(bool flag) { m_ClangEnableModules = flag; }

	inline bool clangEnableObjCARC() const { return m_ClangEnableObjCARC; }
	inline void setClangEnableObjCARC(bool flag) { m_ClangEnableObjCARC = flag; }

	inline XCodeWarnFlag clangWarnBoolConversion() const { return m_ClangWarnBoolConversion; }
	inline void setClangWarnBoolConversion(XCodeWarnFlag flag) { m_ClangWarnBoolConversion = flag; }

	inline XCodeWarnFlag clangWarnConstantConversion() const { return m_ClangWarnConstantConversion; }
	inline void setClangWarnConstantConversion(XCodeWarnFlag flag) { m_ClangWarnConstantConversion = flag; }

	inline XCodeWarnFlag clangWarnDirectObjCIsAUsage() const { return m_ClangWarnDirectObjCIsAUsage; }
	inline void setClangWarnDirectObjCIsAUsage(XCodeWarnFlag flag) { m_ClangWarnDirectObjCIsAUsage = flag; }

	inline XCodeWarnFlag clangWarnEmptyBody() const { return m_ClangWarnEmptyBody; }
	inline void setClangWarnEmptyBody(XCodeWarnFlag flag) { m_ClangWarnEmptyBody = flag; }

	inline XCodeWarnFlag clangWarnEnumConversion() const { return m_ClangWarnEnumConversion; }
	inline void setClangWarnEnumConversion(XCodeWarnFlag flag) { m_ClangWarnEnumConversion = flag; }

	inline XCodeWarnFlag clangWarnIntConversion() const { return m_ClangWarnIntConversion; }
	inline void setClangWarnIntConversion(XCodeWarnFlag flag) { m_ClangWarnIntConversion = flag; }

	inline XCodeWarnFlag clangWarnObjCRootClass() const { return m_ClangWarnObjCRootClass; }
	inline void setClangWarnObjCRootClass(XCodeWarnFlag flag) { m_ClangWarnObjCRootClass = flag; }

	inline XCodeWarnFlag clangWarnDuplicateMethodMatch() const { return m_ClangWarnDuplicateMethodMatch; }
	inline void setClangWarnDuplicateMethodMatch(XCodeWarnFlag flag) { m_ClangWarnDuplicateMethodMatch = flag; }

	inline bool copyPhaseStrip() const { return m_CopyPhaseStrip; }
	inline void setCopyPhaseStrip(bool flag) { m_CopyPhaseStrip = flag; }

	inline const std::string & debugInformationFormat() const { return m_DebugInformationFormat; }
	inline void setDebugInformationFormat(const std::string & fmt) { m_DebugInformationFormat = fmt; }

	inline bool enableNSAssertions() const { return m_EnableNSAssertions; }
	inline void setEnableNSAssertions(bool flag) { m_EnableNSAssertions = flag; }

	inline const std::string & gnuCLanguageStandard() const { return m_GnuCLanguageStandard; }
	inline void setGnuCLanguageStandard(const std::string & std) { m_GnuCLanguageStandard = std; }

	inline bool gccDynamicNoPIC() const { return m_GccDynamicNoPIC; }
	inline void setGccDynamicNoPIC(bool flag) { m_GccDynamicNoPIC = flag; }

	inline bool gccEnableObjCExceptions() const { return m_GccEnableObjCExceptions; }
	inline void setGccEnableObjCExceptions(bool flag) { m_GccEnableObjCExceptions = flag; }

	inline const std::string & gccOptimizationLevel() const { return m_GccOptimizationLevel; }
	inline void setGccOptimizationLevel(const std::string & level) { m_GccOptimizationLevel = level; }

	inline bool gccSymbolsPrivateExtern() const { return m_GccSymbolsPrivateExtern; }
	inline void setGccSymbolsPrivateExtern(bool flag) { m_GccSymbolsPrivateExtern = flag; }

	inline XCodeWarnFlag gccWarn64To32BitConversion() const { return m_GccWarn64To32BitConversion; }
	inline void setGccWarn64To32BitConversion(XCodeWarnFlag flag) { m_GccWarn64To32BitConversion = flag; }

	inline XCodeWarnFlag gccWarnAboutReturnType() const { return m_GccWarnAboutReturnType; }
	inline void setGccWarnAboutReturnType(XCodeWarnFlag flag) { m_GccWarnAboutReturnType = flag; }

	inline XCodeWarnFlag gccWarnUndeclaredSelector() const { return m_GccWarnUndeclaredSelector; }
	inline void setGccWarnUndeclaredSelector(XCodeWarnFlag flag) { m_GccWarnUndeclaredSelector = flag; }

	inline XCodeWarnFlag gccWarnUninitializedAutos() const { return m_GccWarnUninitializedAutos; }
	inline void setGccWarnUninitializedAutos(XCodeWarnFlag flag) { m_GccWarnUninitializedAutos = flag; }

	inline XCodeWarnFlag gccWarnUnusedFunction() const { return m_GccWarnUnusedFunction; }
	inline void setGccWarnUnusedFunction(XCodeWarnFlag flag) { m_GccWarnUnusedFunction = flag; }

	inline XCodeWarnFlag gccWarnUnusedVariable() const { return m_GccWarnUnusedVariable; }
	inline void setGccWarnUnusedVariable(XCodeWarnFlag flag) { m_GccWarnUnusedVariable = flag; }

	inline const std::string & iphoneOSDeploymentTarget() const { return m_IPhoneOSDeploymentTarget; }
	inline void setIPhoneOSDeploymentTarget(const std::string & tgt) { m_IPhoneOSDeploymentTarget = tgt; }

	inline const std::string & macOSXDeploymentTarget() const { return m_MacOSXDeploymentTarget; }
	inline void setMacOSXDeploymentTarget(const std::string & tgt) { m_MacOSXDeploymentTarget = tgt; }

	inline bool onlyActiveArch() const { return m_OnlyActiveArch; }
	inline void setOnlyActiveArch(bool flag) { m_OnlyActiveArch = flag; }

	inline const std::string & productName() const { return m_ProductName; }
	inline void setProductName(const std::string & name_) { m_ProductName = name_; }

	inline const std::string & sdkRoot() const { return m_SDKRoot; }
	inline void setSDKRoot(const std::string & root) { m_SDKRoot = root; }

	inline const std::string & targetedDeviceFamily() const { return m_TargetedDeviceFamily; }
	inline void setTargetedDeviceFamily(const std::string & family) { m_TargetedDeviceFamily = family; }

	inline bool validateProduct() const { return m_ValidateProduct; }
	inline void setValidateProduct(bool flag) { m_ValidateProduct = flag; }

	inline void addPreprocessorDefinition(const std::string & def) { m_Defines.push_back(def); }

	inline void setCodeSignIdentity(const std::string & sdk, const std::string & value)
		{ m_CodeSignIdentitySDK = sdk; m_CodeSignIdentityValue = value; }

protected:
	void writeBuildSettings(std::stringstream & ss) const;

private:
	bool m_AlwaysSearchUserPaths;
	std::string m_Archs;
	std::string m_ClangCxxLanguageStandard;
	std::string m_ClangCxxLibrary;
	bool m_ClangEnableModules;
	bool m_ClangEnableObjCARC;
	XCodeWarnFlag m_ClangWarnBoolConversion;
	XCodeWarnFlag m_ClangWarnConstantConversion;
	XCodeWarnFlag m_ClangWarnDirectObjCIsAUsage;
	XCodeWarnFlag m_ClangWarnEmptyBody;
	XCodeWarnFlag m_ClangWarnEnumConversion;
	XCodeWarnFlag m_ClangWarnIntConversion;
	XCodeWarnFlag m_ClangWarnObjCRootClass;
	XCodeWarnFlag m_ClangWarnDuplicateMethodMatch;
	bool m_CopyPhaseStrip;
	std::string m_DebugInformationFormat;
	bool m_EnableNSAssertions;
	std::string m_GnuCLanguageStandard;
	bool m_GccDynamicNoPIC;
	bool m_GccEnableObjCExceptions;
	std::string m_GccOptimizationLevel;
	bool m_GccSymbolsPrivateExtern;
	XCodeWarnFlag m_GccWarn64To32BitConversion;
	XCodeWarnFlag m_GccWarnAboutReturnType;
	XCodeWarnFlag m_GccWarnUndeclaredSelector;
	XCodeWarnFlag m_GccWarnUninitializedAutos;
	XCodeWarnFlag m_GccWarnUnusedFunction;
	XCodeWarnFlag m_GccWarnUnusedVariable;
	std::string m_IPhoneOSDeploymentTarget;
	std::string m_MacOSXDeploymentTarget;
	bool m_OnlyActiveArch;
	std::string m_ProductName;
	std::string m_SDKRoot;
	std::string m_TargetedDeviceFamily;
	bool m_ValidateProduct;
	std::vector<std::string> m_Defines;
	std::string m_CodeSignIdentitySDK;
	std::string m_CodeSignIdentityValue;

	XCodeProjectBuildConfiguration();
	~XCodeProjectBuildConfiguration();

	static std::string warnFlag(XCodeWarnFlag flag);

	friend class XCodeProject;
};

#endif
