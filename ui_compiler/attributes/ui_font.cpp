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
#include "ui_font.h"
#include "../../util/tinyxml-util/tinyxml-util.h"
#include "../../util/strtod/strtod.h"
#include "../../util/cxx-util/cxx-util/explode.h"
#include "../../util/cxx-util/cxx-util/trim.h"
#include <sstream>
#include <stdexcept>
#include <vector>

UIFontPtr UIFont::fromString(const std::string & str)
{
	std::vector<std::string> values = explode(str, ',');
	if (values.size() != 2)
		throw std::runtime_error("invalid format of the font string.");

	std::vector<std::string> sizes = explode(values[1], '/');
	if (sizes.size() < 1 || sizes.size() > 2)
		throw std::runtime_error("invalid format of the font string.");

	const std::string & family = values[0];
	float size = 0.0f, landscapeSize = 0.0f;

	if (!strToFloat(sizes[0], size))
		throw std::runtime_error("invalid format of the font string.");
	if (!strToFloat((sizes.size() > 1 ? sizes[1] : sizes[0]), landscapeSize))
		throw std::runtime_error("invalid format of the font string.");

	return std::make_shared<UIFont>(family, size, landscapeSize);
}
