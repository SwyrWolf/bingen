cmake_minimum_required(VERSION 4.1.0)

if (WIN32 AND NOT CMAKE_CXX_COMPILER)
	find_program(BINGEN_CLANG clang REQUIRED)
	find_program(BINGEN_CLANGXX clang++ REQUIRED)
	set(CMAKE_C_COMPILER "${BINGEN_CLANG}" CACHE FILEPATH "C compiler")
	set(CMAKE_CXX_COMPILER "${BINGEN_CLANGXX}" CACHE FILEPATH "C++ compiler")
endif()

project(bingen VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	add_compile_options(-Wno-c23-extensions)
endif()

set(SOURCE_FILES src/main.cpp)

set(MODULE_FILES
	src/vend/weretype.cppm
	src/m/coff.cppm
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_sources(${PROJECT_NAME} PRIVATE
	FILE_SET cxx_modules TYPE CXX_MODULES FILES
		${MODULE_FILES}
)

set(TEST_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/test")
set(SQUARE_OBJECT "${TEST_OUTPUT_DIR}/square.o")
add_custom_command(
	OUTPUT "${SQUARE_OBJECT}"
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${TEST_OUTPUT_DIR}"
	COMMAND "$<TARGET_FILE:${PROJECT_NAME}>"
	WORKING_DIRECTORY "${TEST_OUTPUT_DIR}"
	DEPENDS ${PROJECT_NAME}
)

add_executable(bingen_test src/test.cpp "${SQUARE_OBJECT}")
add_dependencies(bingen_test ${PROJECT_NAME})
set_target_properties(bingen_test PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT_DIR}"
)