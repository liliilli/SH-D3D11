cmake_minimum_required (VERSION 3.8)
project(Samples CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQAUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE true)

include_directories($ENV{UniversalCRTSdkDir}/Include/ucrt)
include_directories($ENV{UniversalCRTSdkDir}/Include/10.0.1.0/ucrt)

# https://faithandbrave.hateblo.jp/entry/2014/05/22/174050
add_executable(ShD3D11 WIN32 Source/main.cc)
set_target_properties(ShD3D11 PROPERTIES 
	LINKER_LANGUAGE CXX
)
