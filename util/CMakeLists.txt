
ADD_LIBRARY(util STATIC
	file_type.cpp
	file_type.h
	fmt.h
	path.cpp
	path.h
)

IF(WIN32)
	TARGET_LINK_LIBRARIES(util shlwapi userenv)
ENDIF()
