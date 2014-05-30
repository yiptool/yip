
ADD_LIBRARY(ui_compiler STATIC
	parse_util.cpp
	parse_util.h
	ui_alignment.cpp
	ui_alignment.h
	ui_button.cpp
	ui_button.h
	ui_color.cpp
	ui_color.h
	ui_compiler.cpp
	ui_compiler.h
	ui_group.cpp
	ui_group.h
	ui_image.cpp
	ui_image.h
	ui_label.cpp
	ui_label.h
	ui_layout.cpp
	ui_layout.h
	ui_scale_mode.cpp
	ui_scale_mode.h
	ui_widget.cpp
	ui_widget.h
)

TARGET_LINK_LIBRARIES(ui_compiler tinyxml tinyxml-util util path-util)
