
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

##################################################################################################################
## 3rdparty libraries

MACRO(ADD_SUBMODULE path)
	IF(NOT EXISTS "${path}/CMakeLists.txt")
		EXECUTE_PROCESS(COMMAND
				git submodule update --init --recursive "${path}"
			WORKING_DIRECTORY
				"${CMAKE_SOURCE_DIR}"
		)
	ENDIF()
	ADD_SUBDIRECTORY("${path}")
ENDMACRO()

ADD_SUBMODULE("${CMAKE_SOURCE_DIR}/3rdparty/zlib")
ADD_SUBMODULE("${CMAKE_SOURCE_DIR}/3rdparty/openssl")
ADD_SUBMODULE("${CMAKE_SOURCE_DIR}/3rdparty/libssh2")
ADD_SUBMODULE("${CMAKE_SOURCE_DIR}/3rdparty/libgit2")
ADD_SUBMODULE("${CMAKE_SOURCE_DIR}/3rdparty/inih")

##################################################################################################################
## Compiler flags

INCLUDE(CheckCXXCompilerFlag)

MACRO(CHECK_CXX_FLAG var name)
	CHECK_CXX_COMPILER_FLAG("${name}" HAVE_CXX_FLAG_${var})
	IF(HAVE_CXX_FLAG_${var})
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${name}")
	ENDIF()
ENDMACRO()

IF(MSVC)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE=1)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS=1)
ELSE()
	CHECK_CXX_FLAG(STD_CXX11 "-std=c++11")
	CHECK_CXX_FLAG(PEDANTIC "-pedantic")
	CHECK_CXX_FLAG(WALL "-Wall")
	CHECK_CXX_FLAG(WEXTRA "-Wextra")
	CHECK_CXX_FLAG(WSHADOW "-Wshadow")
	CHECK_CXX_FLAG(WCONVERSION "-Wconversion")
	CHECK_CXX_FLAG(WCXX11_COMPAT "-Wc++11-compat")
ENDIF()

IF(APPLE)
	CHECK_CXX_FLAG(STDLIB_LIBCXX "-stdlib=libc++")
ENDIF()

##################################################################################################################
## Yip

ADD_SUBDIRECTORY(util)
ADD_SUBDIRECTORY(project)
ADD_SUBDIRECTORY(xcode)

ADD_EXECUTABLE(yip
	config.cpp
	config.h
	main.cpp
)

TARGET_LINK_LIBRARIES(yip project xcode util inih libgit2)

INSTALL(TARGETS yip RUNTIME DESTINATION "bin")
