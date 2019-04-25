cmake_minimum_required (VERSION 3.8)
project(ShD3D11 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQAUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE true)

# Add source to this project's executable.

# TODO: Add tests and install targets if needed
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_subdirectory(DyUtils)
add_subdirectory(Samples)