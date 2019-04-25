cmake_minimum_required (VERSION 3.8)
project(InputsBase LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQAUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_VERBOSE_MAKEFILE true)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# Do something 
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	# Do something 
endif()

# Find files
file(GLOB_RECURSE INCLUDE RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/Include" "*.h")
set(SOURCE
	Source/ALowInput.cc
	Source/DInputAnalog.cc
	Source/DInputButton.cc
	Source/DInputMousePos.cc
	Source/XInputEntry.cc
)

# https://stackoverflow.com/questions/2110795/how-to-use-all-c-files-in-a-directory-with-the-cmake-build-system
# AUX_SOURCE_DIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/Source" SOURCE)

# Set static library setting.
include_directories("./Include")

# Add library with include files.
add_library(InputsBase STATIC ${SOURCE})
target_include_directories(InputsBase
PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Include
)

# Add dependencies.
set_target_properties(InputsBase PROPERTIES 
	LINKER_LANGUAGE CXX
	OUTPUT_NAME "InputsBase"
)
target_include_directories(InputsBase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Bind groups
source_group("Include" FILES ${INCLUDE})
source_group("Source" FILES ${SOURCE})

# Install Settings
set_target_properties(InputsBase
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\lib\\${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\lib\\${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\bin\\${CMAKE_BUILD_TYPE}"
)
