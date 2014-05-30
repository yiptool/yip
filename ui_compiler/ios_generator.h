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
#ifndef __2eb5a6e9c487b7a05350099e3a868d01__
#define __2eb5a6e9c487b7a05350099e3a868d01__

#include "../project/project.h"
#include "attributes/ui_font.h"
#include "attributes/ui_scale_mode.h"
#include "parse_util.h"
#include "ui_layout.h"
#include <unordered_map>
#include <string>
#include <sstream>

class UIWidget;

std::string iosScaleFunc(UIScaleMode mode, bool horz);
void iosChooseTranslation(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss,
	const std::string & text);
void iosGetFont(std::stringstream & ss, const UIFontPtr & font, UIScaleMode scaleMode,
	UIScaleMode landscapeScaleMode);
void iosGenerateLayoutCode(const UIWidget * wd, const std::string & prefix, std::stringstream & ss, bool landscape);

void uiGenerateIOSCommonCode(const ProjectPtr & project);
void uiGenerateIOSViewController(UILayoutMap & layouts, const ProjectPtr & project,
	const Project::IOSViewController & cntrl);

#endif
