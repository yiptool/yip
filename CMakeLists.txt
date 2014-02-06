
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ADD_LIBRARY(xcode STATIC
	xcode_group.cpp
	xcode_group.h
	xcode_native_target.cpp
	xcode_native_target.h
	xcode_object.cpp
	xcode_object.h
	xcode_project.cpp
	xcode_project.h
	xcode_unique_id.cpp
	xcode_unique_id.h
)

SET(BUILD_TEST_APP NO CACHE BOOLEAN "Build the test app")
IF(BUILD_TEST_APP)
	ADD_EXECUTABLE(test test.cpp)
	TARGET_LINK_LIBRARIES(test xcode)
ENDIF()
