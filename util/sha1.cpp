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
#include "sha1.h"
#include "../3rdparty/openssl/include/openssl/sha.h"

std::string sha1(const std::string & data)
{
	const char * hex = "0123456789abcdef";
	char buf[40];
	SHA_CTX ctx;

	SHA1_Init(&ctx);
	SHA1_Update(&ctx, data.data(), data.length());
	SHA1_Final(reinterpret_cast<unsigned char *>(buf), &ctx);

	for (int i = 19; i >= 0; i--)
	{
		buf[i * 2 + 1] = hex[static_cast<unsigned char>(buf[i]) & 0xF];
		buf[i * 2 + 0] = hex[static_cast<unsigned char>(buf[i]) >> 4];
	}

	return std::string(buf, 40);
}
