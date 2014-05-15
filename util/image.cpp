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
#include "image.h"
#include "fmt.h"
#include "../3rdparty/libpng/png.h"
#include <new>
#include <stdexcept>
#include <setjmp.h>

bool pngGetInfo(FILE * file, unsigned * outWidth, unsigned * outHeight)
{
	unsigned char sig[8];

	fread(sig, 1, 8, file);
	if (png_sig_cmp(sig, 0, 8) != 0)
		return false;

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png)
		throw std::bad_alloc();

	png_infop info = png_create_info_struct(png);
	if (!info)
	{
		png_destroy_read_struct(&png, nullptr, nullptr);
		throw std::bad_alloc();
	}

	if (setjmp(png_jmpbuf(png)))
	{
		png_destroy_read_struct(&png, &info, nullptr);
		throw std::runtime_error("png file is corrupt.");
	}

	png_init_io(png, file);
	png_set_sig_bytes(png, 8);

	png_read_info(png, info);

	png_uint_32 width = 0, height = 0;
	int bit_depth = 0, color_type = 0;
	png_get_IHDR(png, info, &width, &height, &bit_depth, &color_type, nullptr, nullptr, nullptr);

	if (outWidth)
		*outWidth = static_cast<unsigned>(width);
	if (outHeight)
		*outHeight = static_cast<unsigned>(height);

	png_destroy_read_struct(&png, &info, nullptr);

	return true;
}

namespace
{
	struct AutoFile
	{
		FILE * handle;
		inline AutoFile(FILE * f) : handle(f) {}
		inline ~AutoFile() { fclose(handle); }
	};
}

void imageGetInfo(const std::string & path, ImageFormat * outFormat, unsigned * outWidth, unsigned * outHeight)
{
	FILE * file = fopen(path.c_str(), "rb");
	if (!file)
		throw std::runtime_error(fmt() << "unable to open file '" << path << "'.");

	AutoFile autoFile(file);

	if (pngGetInfo(file, outWidth, outHeight))
	{
		if (outFormat)
			*outFormat = FORMAT_PNG;
		return;
	}

	throw std::runtime_error(fmt() << "image file '" << path << "' has unsupported format.");
}
