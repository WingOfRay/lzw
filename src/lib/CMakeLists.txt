#
# CMakeLists.txt
# author: Jan Dušek <dus3k1an@gmail.com>

set(MUL13_LIB_HEADERS
	arithmcodec.h
)

set(MUL13_LIB_SOURCES
	arithmcodec.cpp
)

add_library(mul13 ${MUL13_LIB_HEADERS} ${MUL13_LIB_SOURCES})
target_link_libraries(mul13)