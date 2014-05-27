
ADD_LIBRARY(ui_compiler STATIC
	ui_compiler.cpp
	ui_compiler.h
)

TARGET_LINK_LIBRARIES(ui_compiler tinyxml tinyxml-util)
