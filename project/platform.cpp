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
#include "platform.h"
#include "../util/fmt.h"
#include <unordered_map>

static std::unordered_map<std::string, Platform::Type> g_PlatformMap;
static std::unordered_map<std::string, BuildType> g_BuildTypeMap;
static bool g_Initialized;

static void init()
{
	if (g_Initialized)
		return;

	g_PlatformMap.insert(std::make_pair("ios", Platform::iOS));
	g_PlatformMap.insert(std::make_pair("android", Platform::Android));
	g_PlatformMap.insert(std::make_pair("osx", Platform::OSX));
	g_PlatformMap.insert(std::make_pair("winrt", Platform::WinRT));
	g_PlatformMap.insert(std::make_pair("qt", Platform::Qt));
	g_PlatformMap.insert(std::make_pair("tizen", Platform::Tizen));
	g_PlatformMap.insert(std::make_pair("nacl", Platform::NaCl));

	g_BuildTypeMap.insert(std::make_pair("debug", BuildType::Debug));
	g_BuildTypeMap.insert(std::make_pair("release", BuildType::Release));

	g_Initialized = true;
}

Platform::Type platformFromString(const std::string & name)
{
	Platform::Type type = platformFromString(name, std::nothrow);
	if (type != Platform::None)
		return type;
	throw std::runtime_error(fmt() << "invalid platform name '" << name << "'.");
}

Platform::Type platformFromString(const std::string & name, const std::nothrow_t &)
{
	init();
	auto it = g_PlatformMap.find(name);
	return (it != g_PlatformMap.end() ? it->second : Platform::None);
}

BuildType buildTypeFromString(const std::string & name)
{
	BuildType type = buildTypeFromString(name, std::nothrow);
	if (type != BuildType::Unspecified)
		return type;
	throw std::runtime_error(fmt() << "invalid build type name '" << name << "'.");
}

BuildType buildTypeFromString(const std::string & name, const std::nothrow_t &)
{
	init();
	auto it = g_BuildTypeMap.find(name);
	return (it != g_BuildTypeMap.end() ? it->second : BuildType::Unspecified);
}
