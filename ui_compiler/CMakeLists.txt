
ADD_LIBRARY(ui_compiler STATIC
	attributes/ui_alignment.cpp
	attributes/ui_alignment.h
	attributes/ui_color.cpp
	attributes/ui_color.h
	attributes/ui_font.cpp
	attributes/ui_font.h
	attributes/ui_image.cpp
	attributes/ui_image.h
	attributes/ui_scale_mode.cpp
	attributes/ui_scale_mode.h
	attributes/ui_text_alignment.cpp
	attributes/ui_text_alignment.h
	widgets/ui_button.cpp
	widgets/ui_button.h
	widgets/ui_group.cpp
	widgets/ui_group.h
	widgets/ui_image_view.cpp
	widgets/ui_image_view.h
	widgets/ui_label.cpp
	widgets/ui_label.h
	widgets/ui_scroll_view.cpp
	widgets/ui_scroll_view.h
	widgets/ui_switch.cpp
	widgets/ui_switch.h
	widgets/ui_text_field.cpp
	widgets/ui_text_field.h
	widgets/ui_webview.cpp
	widgets/ui_webview.h
	widgets/ui_widget.cpp
	widgets/ui_widget.h
	widgets/ui_widget_with_text.cpp
	widgets/ui_widget_with_text.h
	android_generator.cpp
	android_generator.h
	ios_generator.cpp
	ios_generator.h
	parse_util.cpp
	parse_util.h
	ui_compiler.cpp
	ui_compiler.h
	ui_layout.cpp
	ui_layout.h
)

TARGET_LINK_LIBRARIES(ui_compiler tinyxml tinyxml-util util path-util)
