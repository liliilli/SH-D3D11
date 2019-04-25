cmake_minimum_required (VERSION 3.8)
project(NativePlatformBase LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQAUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_VERBOSE_MAKEFILE true)

# Find files
# https://stackoverflow.com/questions/2110795/how-to-use-all-c-files-in-a-directory-with-the-cmake-build-system
file(GLOB_RECURSE INCLUDE RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/Include" "*.h")
AUX_SOURCE_DIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/Source" SOURCE)

# Set static library setting.
include_directories("./Include")

# Add library with include files.
add_library(NativePlatformBase STATIC ${SOURCE})
target_include_directories(NativePlatformBase
PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Include
	${CMAKE_SOURCE_DIR}/DyUtils/DyExpression/Include
	${CMAKE_SOURCE_DIR}/DyUtils/DyMath/Include
	${CMAKE_SOURCE_DIR}/Platform/InputsBase/Include
)
set_target_properties(NativePlatformBase PROPERTIES 
	LINKER_LANGUAGE CXX
	OUTPUT_NAME "NativePlatformBase"
)

# Add dependencies.
target_link_libraries(NativePlatformBase DyMath)

# Bind groups
source_group("Include" FILES ${INCLUDE})
source_group("Source" FILES ${SOURCE})

# Install Settings
set_target_properties(NativePlatformBase
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\lib\\${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\lib\\${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\bin\\${CMAKE_BUILD_TYPE}"
)
