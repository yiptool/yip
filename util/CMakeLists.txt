
ADD_LIBRARY(util STATIC
	cxx_escape.cpp
	cxx_escape.h
	file_type.cpp
	file_type.h
	fmt.h
	git.cpp
	git.h
	path.cpp
	path.h
	sha1.cpp
	sha1.h
	shell.cpp
	shell.h
	sqlite.cpp
	sqlite.h
	xml.cpp
	xml.h
)

IF(WIN32)
	TARGET_LINK_LIBRARIES(util shlwapi userenv)
ENDIF()
