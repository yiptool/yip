
ADD_LIBRARY(project STATIC
	define.cpp
	define.h
	generate_android.cpp
	generate_android.h
	generate_tizen.cpp
	generate_tizen.h
	generate_xcode.cpp
	generate_xcode.h
	header_path.cpp
	header_path.h
	platform.cpp
	platform.h
	project.cpp
	project.h
	project_file_parser.cpp
	project_file_parser.h
	resource_compiler.cpp
	resource_compiler.h
	source_file.cpp
	source_file.h
	ui_compiler.cpp
	ui_compiler.h
	yip_directory.cpp
	yip_directory.h
)

TARGET_LINK_LIBRARIES(project tinyxml-util tinyxml)
