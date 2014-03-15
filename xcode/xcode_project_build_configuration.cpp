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
#include "xcode_project_build_configuration.h"
#include <stdexcept>

XCodeProjectBuildConfiguration::XCodeProjectBuildConfiguration()
	: m_AlwaysSearchUserPaths(false),
	  m_Archs("$(ARCHS_STANDARD_INCLUDING_64_BIT)"),
	  m_ClangCxxLanguageStandard("gnu++11"),
	  m_ClangCxxLibrary("libc++"),
	  m_ClangEnableModules(true),
	  m_ClangEnableObjCARC(false),
	  m_ClangWarnBoolConversion(XCodeEnableWarning),
	  m_ClangWarnConstantConversion(XCodeEnableWarning),
	  m_ClangWarnDirectObjCIsAUsage(XCodeWarningAsError),
	  m_ClangWarnEmptyBody(XCodeEnableWarning),
	  m_ClangWarnEnumConversion(XCodeEnableWarning),
	  m_ClangWarnIntConversion(XCodeEnableWarning),
	  m_ClangWarnObjCRootClass(XCodeWarningAsError),
	  m_ClangWarnDuplicateMethodMatch(XCodeEnableWarning),
	  m_CopyPhaseStrip(false),
	  m_EnableNSAssertions(true),
	  m_GnuCLanguageStandard("gnu99"),
	  m_GccDynamicNoPIC(false),
	  m_GccEnableObjCExceptions(true),
	  m_GccSymbolsPrivateExtern(false),
	  m_GccWarn64To32BitConversion(XCodeEnableWarning),
	  m_GccWarnAboutReturnType(XCodeWarningAsError),
	  m_GccWarnUndeclaredSelector(XCodeEnableWarning),
	  m_GccWarnUninitializedAutos(XCodeEnableWarning),
	  m_GccWarnUnusedFunction(XCodeEnableWarning),
	  m_GccWarnUnusedVariable(XCodeEnableWarning),
	  m_OnlyActiveArch(true),
	  m_ValidateProduct(false)
{
}

XCodeProjectBuildConfiguration::~XCodeProjectBuildConfiguration()
{
}

void XCodeProjectBuildConfiguration::writeBuildSettings(std::stringstream & ss) const
{
	ss << "\t\t\t\tALWAYS_SEARCH_USER_PATHS = " << (m_AlwaysSearchUserPaths ? "YES" : "NO") << ";\n";
	ss << "\t\t\t\tARCHS = " << stringLiteral(m_Archs) << ";\n";
	ss << "\t\t\t\tCLANG_CXX_LANGUAGE_STANDARD = " << stringLiteral(m_ClangCxxLanguageStandard) << ";\n";
	ss << "\t\t\t\tCLANG_CXX_LIBRARY = " << stringLiteral(m_ClangCxxLibrary) << ";\n";
	ss << "\t\t\t\tCLANG_ENABLE_MODULES = " << (m_ClangEnableModules ? "YES" : "NO") << ";\n";
	ss << "\t\t\t\tCLANG_ENABLE_OBJC_ARC = " << (m_ClangEnableObjCARC ? "YES" : "NO") << ";\n";
	ss << "\t\t\t\tCLANG_WARN_BOOL_CONVERSION = " << warnFlag(m_ClangWarnBoolConversion) << ";\n";
	ss << "\t\t\t\tCLANG_WARN_CONSTANT_CONVERSION = " << warnFlag(m_ClangWarnConstantConversion) << ";\n";
	ss << "\t\t\t\tCLANG_WARN_DIRECT_OBJC_ISA_USAGE = " << warnFlag(m_ClangWarnDirectObjCIsAUsage) << ";\n";
	ss << "\t\t\t\tCLANG_WARN_EMPTY_BODY = " << warnFlag(m_ClangWarnEmptyBody) << ";\n";
	ss << "\t\t\t\tCLANG_WARN_ENUM_CONVERSION = " << warnFlag(m_ClangWarnEnumConversion) << ";\n";
	ss << "\t\t\t\tCLANG_WARN_INT_CONVERSION = " << warnFlag(m_ClangWarnIntConversion) << ";\n";
	ss << "\t\t\t\tCLANG_WARN_OBJC_ROOT_CLASS = " << warnFlag(m_ClangWarnObjCRootClass) << ";\n";
	ss << "\t\t\t\tCLANG_WARN__DUPLICATE_METHOD_MATCH = " << warnFlag(m_ClangWarnDuplicateMethodMatch) << ";\n";

	if (m_CodeSignIdentitySDK.length() > 0 || m_CodeSignIdentityValue.length() > 0)
	{
		ss << "\t\t\t\t\"CODE_SIGN_IDENTITY[sdk=" << m_CodeSignIdentitySDK << "]\" = "
			<< stringLiteral(m_CodeSignIdentityValue) << ";\n";
	}

	ss << "\t\t\t\tCOPY_PHASE_STRIP = " << (m_CopyPhaseStrip ? "YES" : "NO") << ";\n";

	if (m_DebugInformationFormat.length() > 0)
		ss << "\t\t\t\tDEBUG_INFORMATION_FORMAT = " << stringLiteral(m_DebugInformationFormat) << ";\n";

	ss << "\t\t\t\tENABLE_NS_ASSERTIONS = " << (m_EnableNSAssertions ? "YES" : "NO") << ";\n";
	ss << "\t\t\t\tGCC_C_LANGUAGE_STANDARD = " << stringLiteral(m_GnuCLanguageStandard) << ";\n";
	ss << "\t\t\t\tGCC_DYNAMIC_NO_PIC = " << (m_GccDynamicNoPIC ? "YES" : "NO") << ";\n";
	ss << "\t\t\t\tGCC_ENABLE_OBJC_EXCEPTIONS = " << (m_GccEnableObjCExceptions ? "YES" : "NO") << ";\n";
	if (m_GccOptimizationLevel.length() > 0)
		ss << "\t\t\t\tGCC_OPTIMIZATION_LEVEL = " << m_GccOptimizationLevel << ";\n";

	if (m_Defines.size() > 0)
	{
		ss << "\t\t\t\tGCC_PREPROCESSOR_DEFINITIONS = (\n";
		for (std::vector<std::string>::const_iterator it = m_Defines.begin(); it != m_Defines.end(); ++it)
			ss << "\t\t\t\t\t" << stringLiteral(*it) << ",\n";
		ss << "\t\t\t\t\t\"$(inherited)\",\n";
		ss << "\t\t\t\t);\n";
	}

	ss << "\t\t\t\tGCC_SYMBOLS_PRIVATE_EXTERN = " << (m_GccSymbolsPrivateExtern ? "YES" : "NO") << ";\n";
	ss << "\t\t\t\tGCC_WARN_64_TO_32_BIT_CONVERSION = " << warnFlag(m_GccWarn64To32BitConversion) << ";\n";
	ss << "\t\t\t\tGCC_WARN_ABOUT_RETURN_TYPE = " << warnFlag(m_GccWarnAboutReturnType) << ";\n";
	ss << "\t\t\t\tGCC_WARN_UNDECLARED_SELECTOR = " << warnFlag(m_GccWarnUndeclaredSelector) << ";\n";
	ss << "\t\t\t\tGCC_WARN_UNINITIALIZED_AUTOS = " << warnFlag(m_GccWarnUninitializedAutos) << ";\n";
	ss << "\t\t\t\tGCC_WARN_UNUSED_FUNCTION = " << warnFlag(m_GccWarnUnusedFunction) << ";\n";
	ss << "\t\t\t\tGCC_WARN_UNUSED_VARIABLE = " << warnFlag(m_GccWarnUnusedVariable) << ";\n";

	if (m_IPhoneOSDeploymentTarget.length() > 0)
		ss << "\t\t\t\tIPHONEOS_DEPLOYMENT_TARGET = " << stringLiteral(m_IPhoneOSDeploymentTarget) << ";\n";

	if (m_LibrarySearchPaths.size() > 0)
	{
		ss << "\t\t\t\tLIBRARY_SEARCH_PATHS = (\n";
		for (const std::string & path : m_LibrarySearchPaths)
			ss << "\t\t\t\t\t" << stringLiteral(path) << ",\n";
		ss << "\t\t\t\t\t\"$(inherited)\",\n";
		ss << "\t\t\t\t);\n";
	}

	if (m_MacOSXDeploymentTarget.length() > 0)
		ss << "\t\t\t\tMACOSX_DEPLOYMENT_TARGET = " << stringLiteral(m_MacOSXDeploymentTarget) << ";\n";

	ss << "\t\t\t\tONLY_ACTIVE_ARCH = " << (m_OnlyActiveArch ? "YES" : "NO") << ";\n";

	if (m_ProductName.length() > 0)
		ss << "\t\t\t\tPRODUCT_NAME = " << stringLiteral(m_ProductName) << ";\n";

	if (m_SDKRoot.length() > 0)
		ss << "\t\t\t\tSDKROOT = " << stringLiteral(m_SDKRoot) << ";\n";

	if (m_TargetedDeviceFamily.length() > 0)
		ss << "\t\t\t\tTARGETED_DEVICE_FAMILY = " << stringLiteral(m_TargetedDeviceFamily) << ";\n";

	ss << "\t\t\t\tVALIDATE_PRODUCT = " << (m_ValidateProduct ? "YES" : "NO") << ";\n";
}

std::string XCodeProjectBuildConfiguration::warnFlag(XCodeWarnFlag flag)
{
	switch (flag)
	{
	case XCodeDisableWarning: return "NO";
	case XCodeEnableWarning: return "YES";
	case XCodeWarningAsError: return "YES_ERROR";
	}
	throw std::runtime_error("invalid value of the warning flag.");
}
