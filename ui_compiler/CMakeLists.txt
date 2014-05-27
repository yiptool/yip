
ADD_LIBRARY(ui_compiler STATIC
	ui_color.cpp
	ui_color.h
	ui_compiler.cpp
	ui_compiler.h
	ui_scale_mode.cpp
	ui_scale_mode.h
	ui_widget.cpp
	ui_widget.h
)

TARGET_LINK_LIBRARIES(ui_compiler tinyxml tinyxml-util)
