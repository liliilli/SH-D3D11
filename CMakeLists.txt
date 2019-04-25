# CMakeList.txt : CMake project for SH-D3D11, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(ShD3D11)

# Add source to this project's executable.

# TODO: Add tests and install targets if needed
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
include_directories($ENV{UniversalCRTSdkDir}/Include/ucrt)
include_directories($ENV{UniversalCRTSdkDir}/Include/10.0.10056.0/ucrt)

# https://faithandbrave.hateblo.jp/entry/2014/05/22/174050
add_executable(example WIN32 src/main.c)
