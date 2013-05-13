#
# CMakeLists.txt
# author: Jan Dušek <jan.dusek90@gmail.com>

include_directories(${PROJECT_SOURCE_DIR}/src/lib)

set(MUL13_LZW_HEADERS
	
)

set(MUL13_LZW_SOURCES
	main.cpp
)

add_executable(lzw ${MUL13_LZW_HEADERS} ${MUL13_LZW_SOURCES})
target_link_libraries(lzw mul13)