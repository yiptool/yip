
ADD_LIBRARY(translation STATIC
	translation_file.cpp
	translation_file.h
)

TARGET_LINK_LIBRARIES(translation tinyxml tinyxml-util util path-util)
