
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ADD_LIBRARY(xcode STATIC
	xcode_build_configuration.cpp
	xcode_build_configuration.h
	xcode_build_file.cpp
	xcode_build_file.h
	xcode_build_phase.cpp
	xcode_build_phase.h
	xcode_configuration_list.cpp
	xcode_configuration_list.h
	xcode_container_item_proxy.cpp
	xcode_container_item_proxy.h
	xcode_file_reference.cpp
	xcode_file_reference.h
	xcode_group.cpp
	xcode_group.h
	xcode_native_target.cpp
	xcode_native_target.h
	xcode_object.cpp
	xcode_object.h
	xcode_project.cpp
	xcode_project.h
	xcode_project_build_configuration.cpp
	xcode_project_build_configuration.h
	xcode_target_build_configuration.cpp
	xcode_target_build_configuration.h
	xcode_target_dependency.cpp
	xcode_target_dependency.h
	xcode_unique_id.cpp
	xcode_unique_id.h
	xcode_variant_group.cpp
	xcode_variant_group.h
)

SET(BUILD_TEST_APP NO CACHE BOOLEAN "Build the test app")
IF(BUILD_TEST_APP)
	ADD_EXECUTABLE(test test.cpp)
	TARGET_LINK_LIBRARIES(test xcode)
ENDIF()
