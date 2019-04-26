cmake_minimum_required (VERSION 3.8)
project(0_HelloWorld CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQAUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE true)

set(D3D11SDK "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)")

include_directories($ENV{UniversalCRTSdkDir}/Include/ucrt)
include_directories($ENV{UniversalCRTSdkDir}/Include/10.0.1.0/ucrt)
AUX_SOURCE_DIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/Source" SOURCE)

# Set Include path.
include_directories("./Include")

# https://faithandbrave.hateblo.jp/entry/2014/05/22/174050
add_executable(0_HelloWorld WIN32 ${SOURCE})
target_include_directories(0_HelloWorld
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/Include
	"${D3D11SDK}/Include"
	
PRIVATE
	${CMAKE_SOURCE_DIR}/DyUtils/DyExpression/Include
	${CMAKE_SOURCE_DIR}/DyUtils/DyStringUtil/Include
	${CMAKE_SOURCE_DIR}/DyUtils/DyMath/Include
	${CMAKE_SOURCE_DIR}/Platform/InputsBase/Include
	${CMAKE_SOURCE_DIR}/Platform/NativePlatformBase/Include	
	${CMAKE_SOURCE_DIR}/Platform/PlatformWin32/Include	
)
set_target_properties(0_HelloWorld PROPERTIES 
	LINKER_LANGUAGE CXX
)
add_definitions(-D_CRT_SECURE_NO_WARNINGS -DUNICODE)

# Add dependencies.
target_link_libraries(0_HelloWorld 
	DyStringUtil
	DyMath
	InputsBase
	PlatformWin32
)
target_link_libraries(0_HelloWorld 
	"${D3D11SDK}/Lib/x64/d3d11.lib"
	"${D3D11SDK}/Lib/x64/dxgi.lib"
	"${D3D11SDK}/Lib/x64/DxErr.lib"
)

if(${CMAKE_BUILD_TYPE} EQUAL "Debug")
	target_link_libraries(0_HelloWorld "${D3D11SDK}/Lib/x64/d3dx11d.lib")
else()
	target_link_libraries(0_HelloWorld "${D3D11SDK}/Lib/x64/d3dx11.lib")
endif()

# Install Settings
set_target_properties(0_HelloWorld
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\lib\\${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\lib\\${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}\\bin\\${CMAKE_BUILD_TYPE}"
)
