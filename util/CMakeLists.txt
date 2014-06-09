
ADD_DEFINITIONS("-DINCLUDE_DIRENT_H=\"${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/dirent/dirent.h\"")

ADD_SUBMODULE("${CMAKE_CURRENT_SOURCE_DIR}/path-util")
ADD_SUBMODULE("${CMAKE_CURRENT_SOURCE_DIR}/cxx-util")
ADD_SUBMODULE("${CMAKE_CURRENT_SOURCE_DIR}/strtod")
ADD_SUBMODULE("${CMAKE_CURRENT_SOURCE_DIR}/tinyxml-util")

ADD_LIBRARY(util STATIC
	cxx_escape.cpp
	cxx_escape.h
	file_type.cpp
	file_type.h
	git.cpp
	git.h
	image.cpp
	image.h
	java_escape.cpp
	java_escape.h
	json_escape.cpp
	json_escape.h
	sha1.cpp
	sha1.h
	shell.cpp
	shell.h
	sqlite.cpp
	sqlite.h
	xml.cpp
	xml.h
)

TARGET_LINK_LIBRARIES(util path-util cxx-util tinyxml-util)
