#
# CMakeLists.txt
# author: Jan Dušek <dus3k1an@gmail.com>

find_package(GTest)
if (GTEST_FOUND)
	enable_testing()
	
	include_directories(${GTEST_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/lib)
	
	# msvc 11 has problem with gtest header file, that can be fixed with this definition
	if (MSVC11)
		add_definitions(/D "_VARIADIC_MAX=10")
	endif (MSVC11)
	
	set(MUL13_TESTS_SOURCES
		TestAC.cpp
		TestLzw.cpp
	)
	
	add_executable(tests ${MUL13_TESTS_SOURCES})
	target_link_libraries(tests mul13 ${GTEST_BOTH_LIBRARIES})
	
	GTEST_ADD_TESTS(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests "" ${MUL13_TESTS_SOURCES})
else()
	message("GTest not found, tests won't be available!")
endif()
