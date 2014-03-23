
Yip
===

Yip is a tool to generate project files and build executables for various
platforms. Yip's primary dedication is to cross-platform game development.

For usage information type `yip help` or `yip --help`.

Compiling Yip
-------------

Yip itself should be compiled with [CMake](http://cmake.org/). Create an
empty directory somewhere on your filesystem, `cd` into it and type:

      cmake <path-to-yip-sources>
      cmake --build .

And CMake will do everything for you (don't forget to replace
`<path-to-yip-sources>` with the actual path to the directory where the source
code of yip is located).

Please consult the
[CMake documentation](http://cmake.org/cmake/help/v2.8.12/cmake.html) for more
information.

Configuration file
------------------

Configuration file is located in the subdirectory `.yip` of the user's home
directory and is named `yip.conf`. This file is created automatically at the
yip's first run.

Project files
-------------

By default yip expects to find a file named `Yipfile` in the directory where
it is run (Project file name could be overriden in the configuration file).

### Source files

The format of the `Yipfile` is pretty simple. Here is an example:

      sources {
         main.cpp
      }

Paths could be either absolute (not recommended) or relative to the directory
where `Yipfile` is located.

You could add a suffix to the `sources` command to specify platforms on which
this source file should be compiled. It could be either a list of allowed
platforms or a list of disallowed platforms:

      sources:ios {
         ios.m
      }
      
      sources:android,tizen {
         android_and_tizen.cpp
      }
      
      sources:!osx {
         not_osx.cpp
      }
      
      sources:!nacl,tizen {
         not_nacl_and_tizen.cpp
      }

File `ios.m` will be compiled only when building for Apple iOS.
File `android_and_tizen.cpp` will be compiled only for Google Android and
Samsung Tizen.
File `not_osx.cpp` will be compiled on all platforms, except Apple Mac OSX.
File `not_nacl_and_tizen.cpp` will be compiled on all platforms, expect Google
PNaCl and Samsung Tizen.

It is possible to specify additional options per source file. Options are
specified after the file name in the form of `name = value` pairs enclosed into
curly braces:

      sources {
         file.c { type = source/objective-c }
      }

Supported options are:

* Option `type` allows to override type of the file. This allows, for example,
  to compile file with the `.c` extension as an Objective-C source.

### Preprocessor definitions

Preprocessor definitions for C family of languages could be specified using
the `defines` directive:

      defines {
         MY_DEFINE
      }

You could limit defines to particular platforms (the syntax is the same as
for the `sources` directive). You could also limit defines to debug or
release builds (use `debug` or `release` as the platform name).

### Importing subprojects

Yip supports importing another projects directly from their git repositories.
It could be achieved by the `import` directive in the `Yipfile`:

      import zlib
      import "http://sqlite.org/copyright.html"

Either git URL or alias could be specified. Alises could be added in the
configuration file.

Imported subprojects will be downloaded only once. To update imported
subprojects later, use the `yip update` command.

### Public headers

Subprojects could make C++ headers available to the main project. For example,
the `zlib` project has the `zlib.h` header file that should be available to any
program using that library.

To handle this case the `public_headers` command could be added to the zlib's
`Yipfile`:

      public_headers
      {
        zlib.h
      }

Now you can include `zlib.h` in your main program:

      #include <yip-imports/zlib.h>

### Resources

Resources could be added into projects using the `resources` directive. Syntax
is the same as for `sources`, even the platform specification suffix is allowed:

      resources
      {
        image.png
        music/cool.ogg
      }
      
      resources:ios
      {
        ios_logo.png
      }

### Application source files

Source files specified in the `source` directive are compiled both when project
is built and when project is imported.

Directive `app_sources` allows to specify source files that will be compiled
only when project is built and will not be compiled when project is imported.

Syntax for `app_sources` directive is the same as for the `sources` directive.

      app_sources
      {
         main.cpp
      }

It is also possible to specify preprocessor definitions to be defined only when
project is build. There is an 'app_defines' directive:

      app_defines
      {
         BUILDING_APP
      }

### Libraries on Microsoft Windows

Libraries on Microsoft Windows could be linked using the `winrt:library`
directive:

      winrt:library
      {
         shell32
         opengl32
      }

### Frameworks on Apple platforms

Frameworks on OSX and iOS could be linked using the `ios:framework` and
`osx:framework` commands respectively. These commands have multiple variants
of syntax:

      osx:framework OpenGL.framework
      osx:framework (libexpat.dylib => "$/usr/lib/libexpat.dylib")
      osx:framework (FacebookSDK.framework => "this/FacebookSDK.framework")

First variant links with the system-provided framework (the ".framework"
suffix in this variant is not necessary and could be omitted). The default
search path is used.

Second variant links with the system-provided framework at the specified path.
The dollar sign at the beginning of the path means *SDKROOT*.

Third variant links with the user-provided framework at the specified path.
Path could be either absolute or relative to the directory where `Yipfile`
is located.

### License handling

License for the project could be specified in the `Yipfile` using the
`license` directive:

      license "Copyright (c) ACME, Inc.
      All rights reserved."

All licenses are collected and are written into the array. This array is
accessible from the application:

      #include <yip/licenses.h>
      
      int main()
      {
          for (size_t i = 0; i < YIP::numLicenses; i++)
              printf("%s\n", YIP::licenses[i]);
          return 0;
      }

Working directory
-----------------

When running `yip` on a project, it will keep all it's files in the `.yip`
subdirectory of the project's directory. This directory **should not** be
redistributed or imported into source control systems like git.

Supported platforms
-------------------

Currently the following platforms are supported:

* Apple Mac OSX (XCode)
* Apple iOS (XCode)

The following platforms are expected to be supported in the near future:

* Microsoft Windows (WinRT, Visual Studio 2013)
* Google Android
* Google PNaCl
* Samsung Tizen
* Qt

License
=======

Copyright © 2014 Nikolay Zapolnov (zapolnov@gmail.com).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

---------

*libgit2* © the libgit2 contributors, unless otherwise stated.
See the `3rdparty/libgit2/AUTHORS` file for details.

*libgit2* is under GPL2 **with linking exemption**. This means you can link to
and use the library from any program, proprietary or open source; paid or
gratis.  However, you cannot modify libgit2 and distribute it without
supplying the source.

See the `3rdparty/libgit2/COPYING` for the full license text.

---------

*libssh2*, a client-side C library implementing the SSH2 protocol.  
Copyright © 2004-2007 Sara Golemon <sarag@libssh2.org>  
Copyright © 2005,2006 Mikhail Gusarov <dottedmag@dottedmag.net>  
Copyright © 2006-2007 The Written Word, Inc.  
Copyright © 2007 Eli Fant <elifantu@mail.ru>  
Copyright © 2009 Daniel Stenberg  
Copyright © 2008, 2009 Simon Josefsson  
All rights reserved.

Redistribution and use in source and binary forms,
with or without modification, are permitted provided
that the following conditions are met:

1. Redistributions of source code must retain the above
   copyright notice, this list of conditions and the
   following disclaimer.

2. Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following
   disclaimer in the documentation and/or other materials
   provided with the distribution.

3. Neither the name of the copyright holder nor the names
   of any other contributors may be used to endorse or
   promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.

---------

The *OpenSSL toolkit* stays under a dual license, i.e. both the conditions of
the OpenSSL License and the original SSLeay license apply to the toolkit.
See file `3rdparty/openssl/LICENSE` for the actual license texts. Actually both
licenses are BSD-style Open Source licenses. In case of any license issues
related to OpenSSL please contact `openssl-core@openssl.org`.

---------

*zlib* © 1995-2013 Jean-loup Gailly and Mark Adler

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

---------

The *inih* library is distributed under the New BSD license:

Copyright © 2009, Brush Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of Brush Technology nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY BRUSH TECHNOLOGY ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL BRUSH TECHNOLOGY BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

---------

The source code of *SQLite3* is in the
[public domain](http://sqlite.org/copyright.html).

The author disclaims copyright to this source code.  In place of
a legal notice, here is a blessing:

* May you do good and not evil.
* May you find forgiveness for yourself and forgive others.
* May you share freely, never taking more than you give.

---------

*Lua* is a powerful, fast, lightweight, embeddable scripting language.  
Copyright © 1994-2013 Lua.org, PUC-Rio.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
