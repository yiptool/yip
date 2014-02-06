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

XCodeProjectBuildConfiguration::XCodeProjectBuildConfiguration()
{
}

XCodeProjectBuildConfiguration::~XCodeProjectBuildConfiguration()
{
}

void XCodeProjectBuildConfiguration::writeBuildSettings(std::stringstream & ss) const
{
/*
+				ALWAYS_SEARCH_USER_PATHS = NO;
+				ARCHS = "$(ARCHS_STANDARD_INCLUDING_64_BIT)";
+				CLANG_CXX_LANGUAGE_STANDARD = "gnu++0x";
+				CLANG_CXX_LIBRARY = "libc++";
+				CLANG_ENABLE_MODULES = YES;
+				CLANG_ENABLE_OBJC_ARC = YES;
+				CLANG_WARN_BOOL_CONVERSION = YES;
+				CLANG_WARN_CONSTANT_CONVERSION = YES;
+				CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;
+				CLANG_WARN_EMPTY_BODY = YES;
+				CLANG_WARN_ENUM_CONVERSION = YES;
+				CLANG_WARN_INT_CONVERSION = YES;
+				CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;
+				CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
+				"CODE_SIGN_IDENTITY[sdk=iphoneos*]" = "iPhone Developer";
+				COPY_PHASE_STRIP = NO;
+				GCC_C_LANGUAGE_STANDARD = gnu99;
+				GCC_DYNAMIC_NO_PIC = NO;
+				GCC_OPTIMIZATION_LEVEL = 0;
+				GCC_PREPROCESSOR_DEFINITIONS = (
+					"DEBUG=1",
+					"$(inherited)",
+				);
+				GCC_SYMBOLS_PRIVATE_EXTERN = NO;
+				GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
+				GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
+				GCC_WARN_UNDECLARED_SELECTOR = YES;
+				GCC_WARN_UNINITIALIZED_AUTOS = YES;
+				GCC_WARN_UNUSED_FUNCTION = YES;
+				GCC_WARN_UNUSED_VARIABLE = YES;
+				IPHONEOS_DEPLOYMENT_TARGET = 7.0;
+				ONLY_ACTIVE_ARCH = YES;
+				SDKROOT = iphoneos;
+				TARGETED_DEVICE_FAMILY = "1,2";
*/
}
