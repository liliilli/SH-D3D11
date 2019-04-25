cmake_minimum_required (VERSION 3.8)
project(PlatformWin32 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQAUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE true)

# Find files
# https://stackoverflow.com/questions/2110795/how-to-use-all-c-files-in-a-directory-with-the-cmake-build-system
file(GLOB_RECURSE INCLUDE RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/Include" "*.h")
AUX_SOURCE_DIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/Source" SOURCE)

# Set static library setting.
include_directories("./Include")

# Add library with include files.
add_library(PlatformWin32 STATIC ${SOURCE})
target_include_directories(PlatformWin32
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/Include
	
PRIVATE
	${CMAKE_SOURCE_DIR}/DyUtils/DyExpression/Include
	${CMAKE_SOURCE_DIR}/DyUtils/DyStringUtil/Include
	${CMAKE_SOURCE_DIR}/DyUtils/DyMath/Include
	${CMAKE_SOURCE_DIR}/Platform/InputsBase/Include
	${CMAKE_SOURCE_DIR}/Platform/NativePlatformBase/Include

	$ENV{UniversalCRTSdkDir}/Include/ucrt
	$ENV{UniversalCRTSdkDir}/Include/10.0.1.0/ucrt
)
set_target_properties(PlatformWin32 PROPERTIES 
	LINKER_LANGUAGE CXX
	OUTPUT_NAME "PlatformWin32"
)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# Add dependencies.
target_link_libraries(PlatformWin32 
	DyMath
	NativePlatformBase
)

# Bind groups
source_group("Include" FILES ${INCLUDE})
source_group("Source" FILES ${SOURCE})

# Install Settings
set_target_properties(PlatformWin32
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\lib\\${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\lib\\${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\bin\\${CMAKE_BUILD_TYPE}"
)
