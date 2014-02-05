
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ADD_LIBRARY(xcode STATIC
	xcode_object.cpp
	xcode_object.h
	xcode_project.cpp
	xcode_project.h
	xcode_unique_id.cpp
	xcode_unique_id.h
)
