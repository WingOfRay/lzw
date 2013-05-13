#
# CMakeLists.txt
# author: Jan Dušek <jan.dusek90@gmail.com>

include_directories(${PROJECT_SOURCE_DIR}/src/lib)

set(MUL13_AC_HEADERS
	
)

set(MUL13_AC_SOURCES
	main.cpp
)

add_executable(ac ${MUL13_AC_HEADERS} ${MUL13_AC_SOURCES})
target_link_libraries(ac mul13)